/****************************************************************************
**
** Copyright (C) 2013-2020 Jolla Ltd.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#include "dbustypes_p.h"
#include "qofonomanager.h"
#include "qofonoutils_p.h"
#include "ofono_manager_interface.h"

class QOfonoManager::Private
{
public:
    OfonoManager *ofonoManager;
    QStringList modems;
    bool available;

    Private() : ofonoManager(NULL), available(false) {}

    QString defaultModem();
    void handleGetModemsReply(QOfonoManager* obj, ObjectPathPropertiesList reply);
    void getModems(QOfonoManager *manager);
};

QString QOfonoManager::Private::defaultModem()
{
    return modems.isEmpty() ? QString() : modems[0];
}

void QOfonoManager::Private::handleGetModemsReply(QOfonoManager* obj, ObjectPathPropertiesList reply)
{
    bool wasAvailable = available;
    QString prevDefault = defaultModem();
    QStringList newModems;
    const int n = reply.count();
    for (int i = 0; i < n; i++) {
        newModems.append(reply.at(i).path.path());
    }
    qSort(newModems);
    available = true;
    if (modems != newModems) {
        modems = newModems;
        Q_EMIT obj->modemsChanged(modems);
    }
    QString newDefault = defaultModem();
    if (newDefault != prevDefault) {
        Q_EMIT obj->defaultModemChanged(newDefault);
    }
    if (!wasAvailable) {
        Q_EMIT obj->availableChanged(true);
    }
}

void QOfonoManager::Private::getModems(QOfonoManager *manager)
{
    if (ofonoManager) {
        connect(new QDBusPendingCallWatcher(
            ofonoManager->GetModems(), ofonoManager),
            SIGNAL(finished(QDBusPendingCallWatcher*)), manager,
            SLOT(onGetModemsFinished(QDBusPendingCallWatcher*)));
    }
}

QOfonoManager::QOfonoManager(QObject *parent) :
    QObject(parent),
    d_ptr(new Private)
{
    QOfonoDbusTypes::registerObjectPathProperties();
    QDBusConnection bus(OFONO_BUS);
    QDBusServiceWatcher *ofonoWatcher = new QDBusServiceWatcher(OFONO_SERVICE, bus,
            QDBusServiceWatcher::WatchForRegistration |
            QDBusServiceWatcher::WatchForUnregistration, this);

    connect(ofonoWatcher, SIGNAL(serviceRegistered(QString)),
            this, SLOT(connectToOfono(QString)));
    connect(ofonoWatcher, SIGNAL(serviceUnregistered(QString)),
            this, SLOT(ofonoUnregistered(QString)));

    if (bus.interface()->isServiceRegistered(OFONO_SERVICE)) {
        connectToOfono(QString());
    }
}

QOfonoManager::~QOfonoManager()
{
    delete d_ptr;
}

QStringList QOfonoManager::modems()
{
    return d_ptr->modems;
}

QStringList QOfonoManager::getModems()
{
    if (d_ptr->ofonoManager && !d_ptr->available) {
        QDBusPendingReply<ObjectPathPropertiesList> reply = d_ptr->ofonoManager->GetModems();
        reply.waitForFinished();
        if (!reply.isError()) {
            d_ptr->handleGetModemsReply(this, reply.value());
        }
    }
    return d_ptr->modems;
}

QString QOfonoManager::defaultModem()
{
    return d_ptr->defaultModem();
}

bool QOfonoManager::available() const
{
    return d_ptr->available;
}

bool QOfonoManager::isValid() const
{
    // isValid() is essentially the same as available(), keeping it around for
    // backward compatibility
    return d_ptr->available;
}

void QOfonoManager::onModemAdded(const QDBusObjectPath& path, const QVariantMap&)
{
    QString pathStr = path.path();
    if (!d_ptr->modems.contains(pathStr)) {
        QString prevDefault = defaultModem();
        d_ptr->modems.append(pathStr);
        qSort(d_ptr->modems);
        Q_EMIT modemAdded(pathStr);
        Q_EMIT modemsChanged(d_ptr->modems);
        QString newDefault = defaultModem();
        if (newDefault != prevDefault) {
            Q_EMIT defaultModemChanged(newDefault);
        }
    }
}

void QOfonoManager::onModemRemoved(const QDBusObjectPath& path)
{
    QString pathStr = path.path();
    QString prevDefault = defaultModem();
    if (d_ptr->modems.removeOne(pathStr)) {
        Q_EMIT modemRemoved(pathStr);
        Q_EMIT modemsChanged(d_ptr->modems);
        QString newDefault = defaultModem();
        if (newDefault != prevDefault) {
            Q_EMIT defaultModemChanged(newDefault);
        }
    }
}

void QOfonoManager::onGetModemsFinished(QDBusPendingCallWatcher* watcher)
{
    QDBusPendingReply<ObjectPathPropertiesList> reply(*watcher);
    watcher->deleteLater();
    if (reply.isError()) {
        if (qofono::isTimeout(reply.error())) {
            qDebug() << "Retrying GetModems...";
            d_ptr->getModems(this);
        } else {
            qWarning() << reply.error();
        }
    } else {
        d_ptr->handleGetModemsReply(this, reply.value());
    }
}

void QOfonoManager::connectToOfono(const QString &)
{
    if (!d_ptr->ofonoManager) {
        OfonoManager* mgr = new OfonoManager(OFONO_SERVICE, "/", OFONO_BUS, this);
        if (mgr->isValid()) {
            d_ptr->ofonoManager = mgr;
            connect(mgr,
                SIGNAL(ModemAdded(QDBusObjectPath,QVariantMap)),
                SLOT(onModemAdded(QDBusObjectPath,QVariantMap)));
            connect(mgr,
                SIGNAL(ModemRemoved(QDBusObjectPath)),
                SLOT(onModemRemoved(QDBusObjectPath)));
            d_ptr->getModems(this);
        } else {
            delete mgr;
        }
    }
}

void QOfonoManager::ofonoUnregistered(const QString &)
{
    if (d_ptr->available) {
        d_ptr->available = false;
        Q_EMIT availableChanged(false);
    }
    if (d_ptr->ofonoManager) {
        delete d_ptr->ofonoManager;
        d_ptr->ofonoManager = NULL;
        if (!d_ptr->modems.isEmpty()) {
            Q_FOREACH(QString modem, d_ptr->modems) {
                Q_EMIT modemRemoved(modem);
            }
            d_ptr->modems.clear();
            Q_EMIT modemsChanged(d_ptr->modems);
            Q_EMIT defaultModemChanged(QString());
        }
    }
}

QSharedPointer<QOfonoManager> QOfonoManager::instance()
{
    static QWeakPointer<QOfonoManager> sharedInstance;
    QSharedPointer<QOfonoManager> mgr = sharedInstance;
    if (mgr.isNull()) {
        mgr = QSharedPointer<QOfonoManager>::create();
        sharedInstance = mgr;
    }
    return mgr;
}
