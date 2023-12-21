/****************************************************************************
**
** Copyright (C) 2014-2020 Jolla Ltd.
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

#include "qofonoobject.h"
#include "qofonoutils_p.h"

namespace {
    const QString GET_PROPERTIES("GetProperties");
    const QString SET_PROPERTY("SetProperty");
}

class QOfonoObject::Private
{
public:
    QOfonoObject::ExtData *ext;
    QDBusAbstractInterface *interface;
    bool initialized;
    bool fixedPath;
    bool validMark;
    int validMarkCount;
    QString objectPath;
    QVariantMap properties;

    Private(QOfonoObject::ExtData *data)
        : ext(data), interface(Q_NULLPTR), initialized(false), fixedPath(false),
          validMark(false), validMarkCount(0) {}
    ~Private() { delete ext; }

    QDBusPendingCall setProperty(const QString &key, const QVariant &value);
    void getProperties(QOfonoObject *obj);

    bool dropDbusInterface();
    void setDbusInterface(QOfonoObject *obj, QDBusAbstractInterface *iface);
    static void applyProperties(QOfonoObject *obj, const QVariantMap &properties);

    class SetPropertyWatcher : public QDBusPendingCallWatcher {
    public:
        QString property;
        SetPropertyWatcher(QDBusAbstractInterface *parent, const QString &name,
            const QDBusPendingCall &call)
            : QDBusPendingCallWatcher(call, parent), property(name) {}
    };
};

QOfonoObject::ExtData::~ExtData()
{
}

QOfonoObject::ValidTracker::ValidTracker(QOfonoObject* obj)
    : object(obj)
{
    if (!(object->d_ptr->validMarkCount++)) {
        object->d_ptr->validMark = obj->isValid();
    }
}

QOfonoObject::ValidTracker::~ValidTracker()
{
    if (!(--object->d_ptr->validMarkCount)) {
        const bool valid = object->isValid();
        if (object->d_ptr->validMark != valid) {
            Q_EMIT object->validChanged(valid);
        }
    }
}

QDBusPendingCall QOfonoObject::Private::setProperty(const QString &key, const QVariant &value)
{
    // Caller checks interface for NULL
    QVariantList args;
    args << QVariant(key) << QVariant::fromValue(QDBusVariant(value));
    return interface->asyncCallWithArgumentList(SET_PROPERTY, args);
}

void QOfonoObject::Private::getProperties(QOfonoObject *obj)
{
    QObject::connect(new QDBusPendingCallWatcher(
        interface->asyncCall(GET_PROPERTIES), interface),
        SIGNAL(finished(QDBusPendingCallWatcher*)), obj,
        SLOT(onGetPropertiesFinished(QDBusPendingCallWatcher*)));
}

bool QOfonoObject::Private::dropDbusInterface()
{
    if (interface) {
        delete interface;
        interface = Q_NULLPTR;
        return true;
    }
    return false;
}

void QOfonoObject::Private::setDbusInterface(QOfonoObject *obj, QDBusAbstractInterface *iface)
{
    interface = iface;
    connect(iface, SIGNAL(PropertyChanged(QString,QDBusVariant)),
            obj, SLOT(onPropertyChanged(QString,QDBusVariant)));
}

void QOfonoObject::Private::applyProperties(QOfonoObject *obj, const QVariantMap &props)
{
    for (QVariantMap::ConstIterator it = props.constBegin(); it != props.constEnd(); ++it) {
        obj->updateProperty(it.key(), it.value());
    }
}

QOfonoObject::QOfonoObject(QObject *parent)
    : QObject(parent),
      d_ptr(new Private(Q_NULLPTR))
{
}

QOfonoObject::QOfonoObject(ExtData *ext, const QString &path, QObject *parent)
    : QObject(parent),
      d_ptr(new Private(ext))
{
    // Just set the path... This constructor is used by the objects which
    // initialize themselves synchronously by calling setDbusInterfaceSync()
    d_ptr->objectPath = path;
}

QOfonoObject::QOfonoObject(ExtData *ext, QObject *parent)
    : QObject(parent),
      d_ptr(new Private(ext))
{
}

QOfonoObject::~QOfonoObject()
{
    delete d_ptr;
}

QOfonoObject::ExtData *QOfonoObject::extData() const
{
    return d_ptr->ext;
}

QString QOfonoObject::objectPath() const
{
    return d_ptr->objectPath;
}

void QOfonoObject::setObjectPath(const QString &path, const QVariantMap *properties)
{
    if (d_ptr->fixedPath) {
        if (d_ptr->objectPath != path) {
            qWarning() << "Attempt to change a fixed path";
        }
    } else if (d_ptr->objectPath != path) {
        d_ptr->objectPath = path;
        objectPathChanged(path, properties);
    }
}

void QOfonoObject::fixObjectPath(const QString &path)
{
    if (d_ptr->fixedPath) {
        qWarning() << "Attempt to fix object path more than once";
    } else if (d_ptr->objectPath != path) {
        d_ptr->objectPath = path;
        d_ptr->fixedPath = true;
        objectPathChanged(path, Q_NULLPTR);
    } else {
        // Still mark it as fixed
        d_ptr->fixedPath = true;
    }
}

void QOfonoObject::objectPathChanged(const QString &, const QVariantMap *properties)
{
    resetDbusInterface(properties);
}

bool QOfonoObject::isValid() const
{
    return d_ptr->interface && d_ptr->interface->isValid() && d_ptr->initialized;
}

QDBusAbstractInterface *QOfonoObject::dbusInterface() const
{
    return d_ptr->interface;
}

bool QOfonoObject::getPropertiesSync() // Since 1.0.101
{
    if (d_ptr->interface) {
        // Synchronous call
        ValidTracker valid(this);
        QDBusPendingReply<QVariantMap> reply(d_ptr->interface->call(GET_PROPERTIES));
        if (reply.isError()) {
            qWarning() << "QOfono GetProperties failure:" << reply.error();
        } else {
            const QVariantMap properties(reply.value());
            Private::applyProperties(this, properties);
            d_ptr->initialized = true;
            return true;
        }
    }
    return false;
}

void QOfonoObject::setDbusInterfaceSync(QDBusAbstractInterface *iface) // Since 1.0.101
{
    ValidTracker valid(this);
    d_ptr->initialized = false;
    if (d_ptr->dropDbusInterface()) {
        dbusInterfaceDropped();
    }
    if (iface) {
        d_ptr->setDbusInterface(this, iface);
        getPropertiesSync();
    }
}

void QOfonoObject::resetDbusInterfaceSync() // Since 1.0.101
{
    setDbusInterfaceSync(d_ptr->objectPath.isEmpty()
                         ? Q_NULLPTR : createDbusInterface(d_ptr->objectPath));
}

void QOfonoObject::setDbusInterface(QDBusAbstractInterface *iface, const QVariantMap *properties)
{
    ValidTracker valid(this);
    d_ptr->initialized = false;
    if (d_ptr->dropDbusInterface()) {
        dbusInterfaceDropped();
    }
    if (iface) {
        d_ptr->setDbusInterface(this, iface);
        if (properties) {
            Private::applyProperties(this, *properties);
            d_ptr->initialized = true;
        } else {
            d_ptr->initialized = false;
            d_ptr->getProperties(this);
        }
    }
}

void QOfonoObject::resetDbusInterface(const QVariantMap *properties)
{
    setDbusInterface(d_ptr->objectPath.isEmpty()
                     ? Q_NULLPTR : createDbusInterface(d_ptr->objectPath), properties);
}

void QOfonoObject::dbusInterfaceDropped()
{
    if (!d_ptr->properties.isEmpty()) {
        const QStringList keys = d_ptr->properties.keys();
        for (int i=0; i<keys.size(); i++) {
            updateProperty(keys.at(i), QVariant());
        }
    }
}

void QOfonoObject::onGetPropertiesFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<QVariantMap> reply(*watch);
    if (!reply.isError()) {
        getPropertiesFinished(reply.value(), Q_NULLPTR);
    } else {
        QDBusError error = reply.error();
        getPropertiesFinished(QVariantMap(), &error);
    }
}

void QOfonoObject::getPropertiesFinished(const QVariantMap &properties, const QDBusError *error)
{
    if (!error) {
        ValidTracker valid(this);
        Private::applyProperties(this, properties);
        d_ptr->initialized = true;
    } else if (qofono::isTimeout(*error)) {
        // Retry GetProperties call if it times out
        qDebug() << "Retrying"
                 << qPrintable(d_ptr->interface->interface() + ".GetProperties")
                 << d_ptr->interface->path();
        d_ptr->getProperties(this);
    } else {
        qWarning() << *error;
        Q_EMIT reportError(error->message());
    }
}

void QOfonoObject::removeProperty(const QString &key)
{
    if (d_ptr->properties.remove(key) > 0) {
        propertyChanged(key, QVariant());
    }
}

QVariantMap QOfonoObject::getProperties() const
{
    return d_ptr->properties;
}

QVariant QOfonoObject::getProperty(const QString &key) const
{
    return d_ptr->properties.value(key);
}

QVariant QOfonoObject::convertProperty(const QString &key, const QVariant &value)
{
    return value;
}

void QOfonoObject::propertyChanged(const QString &key, const QVariant &value)
{
}

void QOfonoObject::updateProperty(const QString &key, const QVariant &value)
{
    QVariant oldValue = getProperty(key);
    QVariant newValue;
    if (value.isValid()) {
        newValue = convertProperty(key, value);
    }
    if (!qofono::safeVariantEq(oldValue, newValue)) {
        if (newValue.isValid()) {
            d_ptr->properties.insert(key, newValue);
        } else {
            d_ptr->properties.remove(key);
        }
        propertyChanged(key, newValue);
    } else if (!oldValue.isValid() && newValue.isValid()) {
        // Not a change, but we need to insert the new value to make isReady() work.
        d_ptr->properties.insert(key, newValue);
    }
}

void QOfonoObject::onPropertyChanged(const QString &key, const QDBusVariant &value)
{
    updateProperty(key, value.variant());
}

bool QOfonoObject::setPropertySync(const QString &key, const QVariant &value)
{
    if (d_ptr->interface) {
        QDBusPendingReply<> reply = d_ptr->setProperty(key, value);
        reply.waitForFinished();
        return reply.isValid() && !reply.isError();
    } else {
        return false;
    }
}

void QOfonoObject::queryProperties()
{
    if (d_ptr->interface) {
        d_ptr->getProperties(this);
    }
}

void QOfonoObject::setProperty(const QString &key, const QVariant &value)
{
    if (d_ptr->interface) {
        connect(new Private::SetPropertyWatcher(d_ptr->interface,
            key, d_ptr->setProperty(key, value)),
            SIGNAL(finished(QDBusPendingCallWatcher*)),
            SLOT(onSetPropertyFinished(QDBusPendingCallWatcher*)));
    }
}

void QOfonoObject::onSetPropertyFinished(QDBusPendingCallWatcher *watch)
{
    watch->deleteLater();
    QDBusPendingReply<> reply(*watch);
    QDBusError dbusError;
    const QDBusError *error;
    if (!reply.isError()) {
        error = Q_NULLPTR;
    } else {
        dbusError = reply.error();
        error = &dbusError;
    }
    setPropertyFinished(((Private::SetPropertyWatcher*)watch)->property, error);
}

void QOfonoObject::setPropertyFinished(const QString &property, const QDBusError *error)
{
    if (error) {
        qDebug() << qPrintable(property) << ": " << *error;
        Q_EMIT reportError(error->message());
    }
    Q_EMIT setPropertyFinished();
}
