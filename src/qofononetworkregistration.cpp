/****************************************************************************
**
** Copyright (C) 2013-2019 Jolla Ltd.
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

#include "qofononetworkregistration.h"
#include "qofononetworkoperator.h"
#include "qofono.h"
#include "ofono_network_registration_interface.h"

#define SUPER QOfonoModemInterface

#define PROPERTY_MODE          QStringLiteral("Mode")
#define PROPERTY_STATUS        QStringLiteral("Status")
#define PROPERTY_LAC           QStringLiteral("LocationAreaCode")
#define PROPERTY_CELL_ID       QStringLiteral("CellId")
#define PROPERTY_MCC           QStringLiteral("MobileCountryCode")
#define PROPERTY_MNC           QStringLiteral("MobileNetworkCode")
#define PROPERTY_TECHNOLOGY    QStringLiteral("Technology")
#define PROPERTY_NAME          QStringLiteral("Name")
#define PROPERTY_STRENGTH      QStringLiteral("Strength")
#define PROPERTY_BASE_STATION  QStringLiteral("BaseStation")

#define STATUS_REGISTERED      QStringLiteral("registered")
#define STATUS_ROAMING         QStringLiteral("roaming")

class QOfonoNetworkRegistration::Private : public QOfonoObject::ExtData
{
public:
    bool initialized;
    bool scanning;
    QOfonoNetworkOperator* currentOperator;
    QHash<QString,QOfonoNetworkOperator*> networkOperators;
    QStringList operatorPaths;
    QString country;

public:
    Private() : initialized(false), scanning(false), currentOperator(NULL) {}
    ~Private() { qDeleteAll(networkOperators.values()); }

    bool updateCountry(QOfonoNetworkRegistration *reg);
};

bool QOfonoNetworkRegistration::Private::updateCountry(QOfonoNetworkRegistration *reg)
{
    const QString status(reg->status());
    QString newCountry;
    if (status == STATUS_REGISTERED || status == STATUS_ROAMING) {
        const int mcc = reg->mcc().toInt();
        if (mcc) {
            newCountry = QOfono::mobileCountryCodeToAlpha2CountryCode(mcc);
        }
    }
    if (country != newCountry) {
        country = newCountry;
        return true;
    } else {
        return false;
    }
}

QOfonoNetworkRegistration::QOfonoNetworkRegistration(QObject *parent) :
    SUPER(OfonoNetworkRegistration::staticInterfaceName(), new Private, parent)
{
    QOfonoDbusTypes::registerObjectPathProperties();
}

QOfonoNetworkRegistration::~QOfonoNetworkRegistration()
{
}

bool QOfonoNetworkRegistration::isValid() const
{
    return privateData()->initialized && SUPER::isValid();
}

bool QOfonoNetworkRegistration::scanning() const
{
    return privateData()->scanning;
}

QDBusAbstractInterface *QOfonoNetworkRegistration::createDbusInterface(const QString &path)
{
    OfonoNetworkRegistration* iface = new OfonoNetworkRegistration("org.ofono", path, QDBusConnection::systemBus(), this);
    iface->setTimeout(120*1000); //increase dbus timeout as scanning can take a long time
    connect(iface,
        SIGNAL(OperatorsChanged(ObjectPathPropertiesList)),
        SLOT(onOperatorsChanged(ObjectPathPropertiesList)));
    connect(new QDBusPendingCallWatcher(iface->GetOperators(), iface),
        SIGNAL(finished(QDBusPendingCallWatcher*)),
        SLOT(onGetOperatorsFinished(QDBusPendingCallWatcher*)));
    return iface;
}

void QOfonoNetworkRegistration::dbusInterfaceDropped()
{
    SUPER::dbusInterfaceDropped();
    Private *d_ptr = privateData();
    d_ptr->initialized = false;
    const bool emitCountryChanged = d_ptr->updateCountry(this);
    bool emitScanningChanged;
    if (d_ptr->scanning) {
        d_ptr->scanning = false;
        emitScanningChanged = true;
    } else {
        emitScanningChanged = true;
    }
    if (!d_ptr->networkOperators.isEmpty()) {
        qDeleteAll(d_ptr->networkOperators.values());
        d_ptr->operatorPaths.clear();
        d_ptr->networkOperators.clear();
        d_ptr->currentOperator = NULL;
        Q_EMIT networkOperatorsChanged(d_ptr->operatorPaths);
    }
    if (emitScanningChanged) {
        Q_EMIT scanningChanged(false);
    }
    if (emitCountryChanged) {
        Q_EMIT countryChanged();
    }
}

void QOfonoNetworkRegistration::registration()
{
    OfonoNetworkRegistration *iface = (OfonoNetworkRegistration*)dbusInterface();
    if (iface) {
        connect(new QDBusPendingCallWatcher(iface->Register(), iface),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(onRegistrationFinished(QDBusPendingCallWatcher*)));
    } else {
        Q_EMIT registrationError(QStringLiteral("Error.ServiceUnknown"));
    }
}

void QOfonoNetworkRegistration::onRegistrationFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<> reply(*watch);
    if (!reply.isError()) {
        Q_EMIT registrationFinished();
    } else {
        Q_EMIT registrationError(reply.error().message());
    }
}

QStringList QOfonoNetworkRegistration::networkOperators() const
{
    return privateData()->operatorPaths;
}

QStringList QOfonoNetworkRegistration::networkOperators()
{
    return privateData()->operatorPaths;
}

QOfonoNetworkOperator* QOfonoNetworkRegistration::networkOperator(const QString &path) const
{
    return privateData()->networkOperators[path];
}

QString QOfonoNetworkRegistration::currentOperatorPath()
{
    Private *d_ptr = privateData();
    return d_ptr->currentOperator ? d_ptr->currentOperator->operatorPath() : QString();
}

void QOfonoNetworkRegistration::scan()
{
    Private *d_ptr = privateData();
    if (!d_ptr->scanning) {
        OfonoNetworkRegistration *iface = (OfonoNetworkRegistration*)dbusInterface();
        if (iface) {
            d_ptr->scanning = true;
            scanningChanged(true);
            connect(new QDBusPendingCallWatcher(iface->Scan(), iface),
                SIGNAL(finished(QDBusPendingCallWatcher*)),
                SLOT(onScanFinished(QDBusPendingCallWatcher*)));
        }
    }
}

QString QOfonoNetworkRegistration::mode() const
{
    return getString(PROPERTY_MODE);
}

QString QOfonoNetworkRegistration::status() const
{
    return getString(PROPERTY_STATUS);
}

uint QOfonoNetworkRegistration::locationAreaCode() const
{
    return getUInt(PROPERTY_LAC);
}

uint QOfonoNetworkRegistration::cellId() const
{
    return getUInt(PROPERTY_CELL_ID);
}

QString QOfonoNetworkRegistration::mcc() const
{
    return getString(PROPERTY_MCC);
}

QString QOfonoNetworkRegistration::mnc() const
{
    return getString(PROPERTY_MNC);
}

QString QOfonoNetworkRegistration::technology() const
{
    return getString(PROPERTY_TECHNOLOGY);
}

QString QOfonoNetworkRegistration::name() const
{
    return getString(PROPERTY_NAME);
}

uint QOfonoNetworkRegistration::strength() const
{
    return getUInt(PROPERTY_STRENGTH);
}

QString QOfonoNetworkRegistration::baseStation() const
{
    return getString(PROPERTY_BASE_STATION);
}

QString QOfonoNetworkRegistration::country() const
{
    return privateData()->country;
}

void QOfonoNetworkRegistration::propertyChanged(const QString &property, const QVariant &value)
{
    SUPER::propertyChanged(property, value);
    if (property == PROPERTY_STRENGTH) {
        Q_EMIT strengthChanged(value.toUInt());
    } else if (property == PROPERTY_MODE) {
        Q_EMIT modeChanged(value.toString());
    } else if (property == PROPERTY_STATUS) {
        if (privateData()->updateCountry(this)) {
            Q_EMIT countryChanged();
        }
        Q_EMIT statusChanged(value.toString());
    } else if (property == PROPERTY_LAC) {
        Q_EMIT locationAreaCodeChanged(value.toUInt());
    } else if (property == PROPERTY_CELL_ID) {
        Q_EMIT cellIdChanged(value.toUInt());
    } else if (property == PROPERTY_MCC) {
        if (privateData()->updateCountry(this)) {
            Q_EMIT countryChanged();
        }
        Q_EMIT mccChanged(value.toString());
    } else if (property == PROPERTY_MNC) {
        Q_EMIT mncChanged(value.toString());
    } else if (property == PROPERTY_TECHNOLOGY) {
        Q_EMIT technologyChanged(value.toString());
    } else if (property == PROPERTY_NAME) {
        Q_EMIT nameChanged(value.toString());
    } else if (property == PROPERTY_BASE_STATION) {
        Q_EMIT baseStationChanged(value.toString());
    }
}

void QOfonoNetworkRegistration::onOperatorsChanged(const ObjectPathPropertiesList &list)
{
    QString oldPath = currentOperatorPath();

    QStringList paths;
    QList<QOfonoNetworkOperator*> newOperators;
    QOfonoNetworkOperator* currentOperator = NULL;
    Private *d_ptr = privateData();
    bool listChanged = false;
    int i;

    // Find new operators
    for (i=0; i<list.count(); i++) {
        QString path = list[i].path.path();
        paths.append(path);
        if (!d_ptr->networkOperators.contains(path)) {
            // No need to query the properties as we already have the.
            // The object becomes valid immediately.
            QOfonoNetworkOperator* op = new QOfonoNetworkOperator(path,
                list[i].properties, this);
            newOperators.append(op);
            connect(op, SIGNAL(statusChanged(QString)),
                SLOT(onOperatorStatusChanged(QString)));
            if (op->status() == "current") {
                currentOperator = op;
            }
            listChanged = true;
        }
    }

    // Remove operators that are no longer on the list
    for (i=d_ptr->operatorPaths.count()-1; i>=0; i--) {
        QString path = d_ptr->operatorPaths[i];
        if (!paths.contains(path)) {
            QOfonoNetworkOperator* op = d_ptr->networkOperators[path];
            if (op == d_ptr->currentOperator) d_ptr->currentOperator = NULL;
            d_ptr->operatorPaths.removeAt(i);
            d_ptr->networkOperators.remove(path);
            delete op;
            listChanged = true;
        }
    }

    // Append new operators to the end of the list
    for (i=0; i<newOperators.count(); i++) {
        QOfonoNetworkOperator* op = newOperators[i];
        d_ptr->operatorPaths.append(op->operatorPath());
        d_ptr->networkOperators.insert(op->operatorPath(), op);
    }

    // Replace the current operator if it has changed
    if (currentOperator) d_ptr->currentOperator = currentOperator;

    // Fire necessary events
    if (listChanged) {
        Q_EMIT networkOperatorsChanged(d_ptr->operatorPaths);
    }
    QString currentPath = currentOperatorPath();
    if (currentPath != oldPath) {
        Q_EMIT currentOperatorPathChanged(currentPath);
    }
}

void QOfonoNetworkRegistration::onGetOperatorsFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<ObjectPathPropertiesList> reply(*watch);
    if (reply.isError()) {
        qDebug() << reply.error();
        Q_EMIT reportError(reply.error().message());
    } else {
        ValidTracker valid(this);
        privateData()->initialized = true;
        onOperatorsChanged(reply.value());
    }
}

void QOfonoNetworkRegistration::onScanFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<ObjectPathPropertiesList> reply(*watch);
    if (reply.isError()) {
        qDebug() << reply.error();
        Q_EMIT scanError(reply.error().message());
    } else {
        onOperatorsChanged(reply.value());
        Q_EMIT scanFinished();
    }
    privateData()->scanning = false;
    scanningChanged(false);
}

void QOfonoNetworkRegistration::onOperatorStatusChanged(const QString &status)
{
    QString oldPath = currentOperatorPath();
    QOfonoNetworkOperator *op = (QOfonoNetworkOperator*)sender();
    Private *d_ptr = privateData();
    if (status == "current") {
        d_ptr->currentOperator = op;
    } else if (d_ptr->currentOperator == op) {
        d_ptr->currentOperator = NULL;
    }
    QString currentPath = currentOperatorPath();
    if (currentPath != oldPath) {
        Q_EMIT currentOperatorPathChanged(currentPath);
    }
}

QString QOfonoNetworkRegistration::modemPath() const
{
    return SUPER::modemPath();
}

void QOfonoNetworkRegistration::setModemPath(const QString &path)
{
    SUPER::setModemPath(path);
}

QOfonoNetworkRegistration::Private *QOfonoNetworkRegistration::privateData() const
{
    return (QOfonoNetworkRegistration::Private*)SUPER::extData();
}
