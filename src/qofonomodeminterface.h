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

#ifndef QOFONOMODEMINTERFACE_H
#define QOFONOMODEMINTERFACE_H

#include "qofonoobject.h"
#include "qofono_global.h"

/**
 * Modem interface with properties.
 */
class QOFONOSHARED_EXPORT QOfonoModemInterface : public QOfonoObject
{
    Q_OBJECT
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath NOTIFY modemPathChanged)
    Q_PROPERTY(bool ready READ isReady NOTIFY readyChanged)

protected:
    QOfonoModemInterface(const QString &iface, ExtData *ext, QObject *parent = NULL);
    QOfonoModemInterface(const QString &iface, QObject *parent = NULL);
    ~QOfonoModemInterface();

    virtual ExtData *extData() const Q_DECL_OVERRIDE;

    bool isReady() const;

public:
    QString modemPath() const;
    void setModemPath(const QString &path);
    void fixModemPath(const QString &path);

    bool isValid() const Q_DECL_OVERRIDE;

Q_SIGNALS:
    void modemPathChanged(const QString &path);
    void readyChanged();

protected:
    void getPropertiesFinished(const QVariantMap &properties, const QDBusError *error) Q_DECL_OVERRIDE;
    void updateProperty(const QString &key, const QVariant &value) Q_DECL_OVERRIDE;
    void objectPathChanged(const QString &path, const QVariantMap *properties) Q_DECL_OVERRIDE;

private Q_SLOTS:
    void onModemInterfacesChanged(const QStringList &interfaces);
    void onModemValidChanged(bool valid);

private:
    class Private;
    Private* privateData() const;
};

#endif // QOFONOMODEMINTERFACE_H
