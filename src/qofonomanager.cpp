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
    typedef void (Private::*GetModems)(QOfonoManager *);

    OfonoManager *ofonoManager;
    QStringList modems;
    bool available;

    Private();

    QString defaultModem();
    void setup(QOfonoManager *obj, GetModems getModems);
    void connectToOfono(QOfonoManager *obj, GetModems getModems);
    void handleGetModemsReply(QOfonoManager *obj, ObjectPathPropertiesList reply);
    void getModems(QOfonoManager *obj);
    void getModemsSync(QOfonoManager *obj);
};

QOfonoManager::Private::Private() :
    ofonoManager(Q_NULLPTR),
    available(false)
{
    QOfonoDbusTypes::registerObjectPathProperties();
}

QString QOfonoManager::Private::defaultModem()
{
    return modems.isEmpty() ? QString() : modems[0];
}

void QOfonoManager::Private::setup(QOfonoManager *obj, GetModems getModems)
{
    QDBusConnection bus(OFONO_BUS);
    QDBusServiceWatcher *ofonoWatcher = new QDBusServiceWatcher(OFONO_SERVICE, bus,
            QDBusServiceWatcher::WatchForRegistration |
            QDBusServiceWatcher::WatchForUnregistration, obj);

    obj->connect(ofonoWatcher, SIGNAL(serviceRegistered(QString)),
        SLOT(connectToOfono(QString)));
    obj->connect(ofonoWatcher, SIGNAL(serviceUnregistered(QString)),
        SLOT(ofonoUnregistered(QString)));

    if (bus.interface()->isServiceRegistered(OFONO_SERVICE)) {
        connectToOfono(obj, getModems);
    }
}

void QOfonoManager::Private::connectToOfono(QOfonoManager *obj, GetModems getModems)
{
    if (!ofonoManager) {
        OfonoManager *mgr = new OfonoManager(OFONO_SERVICE, "/", OFONO_BUS, obj);
        if (mgr->isValid()) {
            ofonoManager = mgr;
            obj->connect(mgr,
                SIGNAL(ModemAdded(QDBusObjectPath,QVariantMap)),
                SLOT(onModemAdded(QDBusObjectPath,QVariantMap)));
            obj->connect(mgr,
                SIGNAL(ModemRemoved(QDBusObjectPath)),
                SLOT(onModemRemoved(QDBusObjectPath)));
            (this->*getModems)(obj);
        } else {
            delete mgr;
        }
    }
}

void QOfonoManager::Private::handleGetModemsReply(QOfonoManager *obj, ObjectPathPropertiesList reply)
{
    const bool wasAvailable = available;
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

void QOfonoManager::Private::getModemsSync(QOfonoManager *obj)
{
    if (ofonoManager) {
        QDBusPendingReply<ObjectPathPropertiesList> reply = ofonoManager->GetModems();
        reply.waitForFinished();
        if (reply.isError()) {
            qWarning() << reply.error();
        } else {
            handleGetModemsReply(obj, reply.value());
        }
    }
}

QOfonoManager::QOfonoManager(QObject *parent) :
    QObject(parent),
    d_ptr(new Private)
{
    d_ptr->setup(this, &Private::getModems);
}

QOfonoManager::QOfonoManager(bool mayBlock, QObject *parent) : // Since 1.0.101
    QObject(parent),
    d_ptr(new Private)
{
    d_ptr->setup(this, mayBlock ? &Private::getModemsSync :  &Private::getModems);
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
    if (!d_ptr->available) {
        d_ptr->getModemsSync(this);
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
    // This is equivalent to available() for historical reasons
    return d_ptr->available;
}

void QOfonoManager::onModemAdded(const QDBusObjectPath &path, const QVariantMap&)
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

void QOfonoManager::onModemRemoved(const QDBusObjectPath &path)
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

void QOfonoManager::onGetModemsFinished(QDBusPendingCallWatcher *watcher)
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
        OfonoManager *mgr = new OfonoManager(OFONO_SERVICE, "/", OFONO_BUS, this);
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
        d_ptr->ofonoManager = Q_NULLPTR;
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
    return instance(false); // Don't block
}

QSharedPointer<QOfonoManager> QOfonoManager::instance(bool mayBlock)
{
    static QWeakPointer<QOfonoManager> sharedInstance;
    QSharedPointer<QOfonoManager> mgr = sharedInstance;
    if (mgr.isNull()) {
        mgr = QSharedPointer<QOfonoManager>(new QOfonoManager(mayBlock), &QObject::deleteLater);
        sharedInstance = mgr;
    } else if (mayBlock) {
        // Make sure that modems are available
        mgr->getModems();
    }
    return mgr;
}
