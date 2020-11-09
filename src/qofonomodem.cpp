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
#include "qofonomodem.h"
#include "qofonomanager.h"
#include "ofono_modem_interface.h"

#include <QMap>
#include <QWeakPointer>

typedef QMap<QString,QWeakPointer<QOfonoModem> > ModemMap;
Q_GLOBAL_STATIC(ModemMap, modemMap)

#define SUPER QOfonoObject
#define MODEM_PROPERTIES(p) \
    p(Online) \
    p(Powered) \
    p(Lockdown) \
    p(Emergency) \
    p(Name) \
    p(Manufacturer) \
    p(Model) \
    p(Revision) \
    p(Serial) \
    p(Type) \
    p(SoftwareVersionNumber) \
    p(Features) \
    p(Interfaces) \

class QOfonoModem::Private : public SUPER::ExtData
{
public:
#define DECLARE_PROPERTY(p) static const QString p;
    MODEM_PROPERTIES(DECLARE_PROPERTY)

    bool modemPathValid;
    QSharedPointer<QOfonoManager> mgr;

    Private(QSharedPointer<QOfonoManager> manager) : modemPathValid(false), mgr(manager) {}
    void setup(QOfonoModem *modem);
    bool isModemPathValid(const QString &path);
};

inline void QOfonoModem::Private::setup(QOfonoModem *modem)
{
    connect(mgr.data(), SIGNAL(availableChanged(bool)), modem, SLOT(checkModemPathValidity()));
    connect(mgr.data(), SIGNAL(modemsChanged(QStringList)), modem, SLOT(checkModemPathValidity()));
}

inline bool QOfonoModem::Private::isModemPathValid(const QString &path)
{
    return !path.isEmpty() && mgr->isValid() && mgr->modems().contains(path);
}

#define DEFINE_PROPERTY(p) const QString QOfonoModem::Private::p(QLatin1String(#p));
MODEM_PROPERTIES(DEFINE_PROPERTY)

QOfonoModem::QOfonoModem(QObject *parent) :
    SUPER(new Private(QOfonoManager::instance(false)), parent)
{
    privateData()->setup(this);
    checkModemPathValidity();
}

QOfonoModem::QOfonoModem(const QString &path, QObject *parent) : // Since 1.0.101
    SUPER(new Private(QOfonoManager::instance(true)), path, parent)
{
    Private* priv = privateData();
    priv->setup(this);
    priv->modemPathValid = priv->isModemPathValid(path);
    if (priv->modemPathValid) {
        resetDbusInterfaceSync();
    }
    if (!isValid()) {
        // Try to repeat the call asynchronously if something went wrong.
        // That may help in case if it was a timeout
        queryProperties();
    }
}

QOfonoModem::~QOfonoModem()
{
}

QDBusAbstractInterface *QOfonoModem::createDbusInterface(const QString &path)
{
    return new OfonoModem(OFONO_SERVICE, path, OFONO_BUS, this);
}

void QOfonoModem::objectPathChanged(const QString &path, const QVariantMap *properties)
{
    Q_EMIT modemPathChanged(path);
    if (!checkModemPathValidity()) {
        // checkModemPathValidity() didn't do anything because modemPathValid
        // flag hasn't changed. If the modem path has changed from one valid
        // path to another, D-Bus interface has to be re-initialized.
        if (privateData()->modemPathValid) {
            resetDbusInterface(properties);
        }
    }
}

void QOfonoModem::setModemPath(const QString &path)
{
    setObjectPath(path);
}

QString QOfonoModem::modemPath() const
{
    return objectPath();
}

bool QOfonoModem::powered() const
{
    return getBool(Private::Powered);
}

bool QOfonoModem::online() const
{
    return getBool(Private::Online);
}

bool QOfonoModem::lockdown() const
{
    return getBool(Private::Lockdown);
}

bool QOfonoModem::emergency() const
{
    return getBool(Private::Emergency);
}

QString QOfonoModem::name() const
{
    return getString(Private::Name);
}

QString QOfonoModem::manufacturer() const
{
    return getString(Private::Manufacturer);
}

QString QOfonoModem::model() const
{
    return getString(Private::Model);
}

QString QOfonoModem::revision() const
{
    return getString(Private::Revision);
}

QString QOfonoModem::serial() const
{
    return getString(Private::Serial);
}

QString QOfonoModem::type() const
{
    return getString(Private::Type);
}

QString QOfonoModem::softwareVersionNumber() const
{
    return getString(Private::SoftwareVersionNumber);
}

QStringList QOfonoModem::features() const
{
    return getStringList(Private::Features);
}

QStringList QOfonoModem::interfaces() const
{
    return getStringList(Private::Interfaces);
}

void QOfonoModem::setPowered(bool powered)
{
    setProperty(Private::Powered, powered);
}

void QOfonoModem::setOnline(bool online)
{
    setProperty(Private::Online, online);
}

void QOfonoModem::setLockdown(bool lockdown)
{
    setProperty(Private::Lockdown, lockdown);
}

void QOfonoModem::propertyChanged(const QString &property, const QVariant &value)
{
    SUPER::propertyChanged(property, value);
    if (property == Private::Online) {
        Q_EMIT onlineChanged(value.toBool());
    } else if (property == Private::Powered) {
        Q_EMIT poweredChanged(value.toBool());
    } else if (property == Private::Lockdown) {
        Q_EMIT lockdownChanged(value.toBool());
    } else if (property == Private::Emergency) {
        Q_EMIT emergencyChanged(value.toBool());
    } else if (property == Private::Name) {
        Q_EMIT nameChanged(value.toString());
    } else if (property == Private::Manufacturer) {
        Q_EMIT manufacturerChanged(value.toString());
    } else if (property == Private::Model) {
        Q_EMIT modelChanged(value.toString());
    } else if (property == Private::Revision) {
        Q_EMIT revisionChanged(value.toString());
    } else if (property == Private::Serial) {
        Q_EMIT serialChanged(value.toString());
    } else if (property == Private::Type) {
        Q_EMIT typeChanged(value.toString());
    } else if (property == Private::SoftwareVersionNumber) {
        Q_EMIT softwareVersionNumberChanged(value.toString());
    } else if (property == Private::Features) {
        Q_EMIT featuresChanged(value.toStringList());
    } else if (property == Private::Interfaces) {
        Q_EMIT interfacesChanged(value.toStringList());
    }
}

QSharedPointer<QOfonoModem> QOfonoModem::instance(const QString &modemPath)
{
    return instance(modemPath, false);
}

QSharedPointer<QOfonoModem> QOfonoModem::instance(const QString &modemPath, bool mayBlock) // Since 1.0.101
{
    QSharedPointer<QOfonoModem> modem = modemMap()->value(modemPath);
    if (modem.isNull()) {
        modem = QSharedPointer<QOfonoModem>(mayBlock ? new QOfonoModem(modemPath) :
            new QOfonoModem(), &QObject::deleteLater);
        modem->fixObjectPath(modemPath);
        modemMap()->insert(modemPath, QWeakPointer<QOfonoModem>(modem));
    } else if (mayBlock && !modem->isValid()) {
        // This may generate a second (synchronous) GetProperties call for
        // the same object (if the first call was asynchronous and hasn't
        // compeleted yet) but that should be a fairly rare case, not worth
        // optimization.
        modem->getPropertiesSync();
    }
    return modem;
}

bool QOfonoModem::isValid() const
{
    return SUPER::isValid() && privateData()->modemPathValid;
}

QOfonoModem::Private *QOfonoModem::privateData() const
{
    return (Private*)SUPER::extData();
}

bool QOfonoModem::checkModemPathValidity()
{
    ValidTracker valid(this);
    Private *priv = privateData();
    const bool modemPathValid = priv->isModemPathValid(modemPath());
    if (priv->modemPathValid != modemPathValid) {
        priv->modemPathValid = modemPathValid;
        if (modemPathValid) {
            resetDbusInterface();
        } else {
            setDbusInterface(NULL);
        }
        return true;
    } else {
        // Return false to indicate that this function didn't do anything
        // because modemPathValid hasn't changed
        return false;
    }
}
