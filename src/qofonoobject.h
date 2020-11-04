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

#ifndef QOFONOOBJECT_H
#define QOFONOOBJECT_H

#include "dbustypes.h"
#include "qofono_global.h"

class QOFONOSHARED_EXPORT QOfonoObject : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)

protected:
    friend class ValidTracker;
    class ValidTracker {
    private:
        QOfonoObject* object;
    public:
        ValidTracker(QOfonoObject* object);
        ~ValidTracker();
    };

public:
    class ExtData {
    public:
        virtual ~ExtData();
    };

protected:
    QOfonoObject(ExtData *ext, const QString &path, QObject *parent = Q_NULLPTR);  // Since 1.0.101
    QOfonoObject(ExtData *ext, QObject *parent = Q_NULLPTR);
    QOfonoObject(QObject *parent = Q_NULLPTR);
    ~QOfonoObject();

    virtual ExtData *extData() const;

public:
    QString objectPath() const;
    void setObjectPath(const QString &path, const QVariantMap *properties = Q_NULLPTR);
    bool getPropertiesSync(); // Since 1.0.101
    virtual bool isValid() const;

Q_SIGNALS:
    void validChanged(bool valid);
    void setPropertyFinished();
    void reportError(const QString &errorString);

protected:
    virtual void objectPathChanged(const QString &path, const QVariantMap *properties) = 0;
    virtual QDBusAbstractInterface *createDbusInterface(const QString &path) = 0;
    virtual void dbusInterfaceDropped();

    virtual QVariant convertProperty(const QString &key, const QVariant &value);
    virtual void updateProperty(const QString &key, const QVariant &value);
    virtual void propertyChanged(const QString &key, const QVariant &value);
    virtual void getPropertiesFinished(const QVariantMap &properties, const QDBusError *error);
    virtual void setPropertyFinished(const QString &property, const QDBusError *error);

    void setProperty(const QString &key, const QVariant &value);
    bool setPropertySync(const QString &key, const QVariant &value);
    void queryProperties();

    void removeProperty(const QString &key);
    QVariantMap getProperties() const;
    QVariant getProperty(const QString &key) const;
    QString getString(const QString &key) const;
    QStringList getStringList(const QString &key) const;
    QVariantMap getVariantMap(const QString &key) const;
    QVariantList getVariantList(const QString &key) const;
    bool getBool(const QString &key) const;
    uint getUInt(const QString &key) const;
    int getInt(const QString &key) const;

    QDBusAbstractInterface *dbusInterface() const;
    void setDbusInterfaceSync(QDBusAbstractInterface *iface);  // Since 1.0.101
    void resetDbusInterfaceSync();  // Since 1.0.101
    void setDbusInterface(QDBusAbstractInterface *iface, const QVariantMap *properties = Q_NULLPTR);
    void resetDbusInterface(const QVariantMap *properties = Q_NULLPTR);
    void fixObjectPath(const QString &path);

private slots:
    void onGetPropertiesFinished(QDBusPendingCallWatcher *watch);
    void onSetPropertyFinished(QDBusPendingCallWatcher *watch);
    void onPropertyChanged(const QString &key, const QDBusVariant &value);

private:
    class Private;
    Private *d_ptr;
};

inline QString QOfonoObject::getString(const QString &key) const
    { return getProperty(key).toString(); }
inline QStringList QOfonoObject::getStringList(const QString &key) const
    { return getProperty(key).toStringList(); }
inline QVariantMap QOfonoObject::getVariantMap(const QString &key) const
    { return getProperty(key).value<QVariantMap>(); }
inline QVariantList QOfonoObject::getVariantList(const QString &key) const
    { return getProperty(key).value<QVariantList>(); }
inline bool QOfonoObject::getBool(const QString &key) const
    { return getProperty(key).toBool(); }
inline uint QOfonoObject::getUInt(const QString &key) const
    { return getProperty(key).toUInt(); }
inline int QOfonoObject::getInt(const QString &key) const
    { return getProperty(key).toInt(); }

#endif // QOFONOOBJECT_H
