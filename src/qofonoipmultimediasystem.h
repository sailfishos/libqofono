/****************************************************************************
**
** Copyright (C) 2022 Jolla Ltd.
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

#ifndef QOFONOIpMultimediaSystem_H
#define QOFONOIpMultimediaSystem_H

#include "qofonomodeminterface.h"
#include "qofono_global.h"

/*!
 * oFono IMS API is documented in
 * https://git.kernel.org/pub/scm/network/ofono/ofono.git/plain/doc/ims-api.txt
 * https://github.com/sailfishos/ofono/blob/master/ofono/doc/ims-api.txt
 */
class QOFONOSHARED_EXPORT QOfonoIpMultimediaSystem : public QOfonoModemInterface
{
    Q_OBJECT
    Q_ENUMS(Registration)
    Q_PROPERTY(Registration registration READ registration WRITE setRegistration NOTIFY registrationChanged)
    Q_PROPERTY(bool registered READ registered WRITE setRegistered NOTIFY registeredChanged)
    Q_PROPERTY(bool voiceCapable READ voiceCapable NOTIFY voiceCapableChanged)
    Q_PROPERTY(bool smsCapable READ smsCapable NOTIFY smsCapableChanged)
    class Interface;

public:
    enum Registration {
        RegistrationUnknown,
        RegistrationDisabled,
        RegistrationEnabled,
        RegistrationAuto
    };

    explicit QOfonoIpMultimediaSystem(QObject *parent = Q_NULLPTR);
    ~QOfonoIpMultimediaSystem();

    Registration registration() const;
    void setRegistration(Registration value);

    void setRegistered(bool registered);
    bool registered() const;
    bool voiceCapable() const;
    bool smsCapable() const;

    bool registerSync(QDBusError *error = Q_NULLPTR);
    bool unregisterSync(QDBusError *error = Q_NULLPTR);

Q_SIGNALS:
    void registrationChanged();
    void registeredChanged();
    void voiceCapableChanged();
    void smsCapableChanged();

protected:
    QDBusAbstractInterface *createDbusInterface(const QString &path) Q_DECL_OVERRIDE;
    void propertyChanged(const QString &key, const QVariant &value) Q_DECL_OVERRIDE;
};

#endif // QOFONOIpMultimediaSystem_H
