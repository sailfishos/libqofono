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

#include "dbustypes_p.h"
#include "qofonoipmultimediasystem.h"

// ==========================================================================
// QOfonoIpMultimediaSystem::Proxy
//
// https://git.kernel.org/pub/scm/network/ofono/ofono.git/plain/doc/ims-api.txt
//
// <node>
//   <interface name="org.ofono.IpMultimediaSystem">
//     <method name="GetProperties">
//       <arg name="properties" type="a{sv}" direction="out"/>
//     </method>
//     <method name="Register"/>
//     <method name="Unregister"/>
//     <signal name="PropertyChanged">
//       <arg name="name" type="s"/>
//       <arg name="value" type="v"/>
//     </signal>
//   </interface>
// </node>
//
// ==========================================================================

class QOfonoIpMultimediaSystem::Interface: public QDBusAbstractInterface
{
    Q_OBJECT

public:
    static const char NAME[];
    static const QString METHOD_REGISTER;
    static const QString METHOD_UNREGISTER;
    static const QString PROP_REGISTERED;
    static const QString PROP_VOICE_CAPABLE;
    static const QString PROP_SMS_CAPABLE;

    Interface(QString aPath, QObject* aParent) :
        QDBusAbstractInterface(OFONO_SERVICE, aPath, NAME, OFONO_BUS, aParent) {}

    static bool syncCall(Interface *proxy, const QString &method, QDBusError *outError);

public Q_SLOTS: // METHODS
    QDBusPendingCall Register() { return asyncCall(METHOD_REGISTER); }
    QDBusPendingCall Unregister() { return asyncCall(METHOD_UNREGISTER); }

Q_SIGNALS: // SIGNALS
    void PropertyChanged(const QString &name, const QDBusVariant &value);
};

const char QOfonoIpMultimediaSystem::Interface::NAME[] = "org.ofono.IpMultimediaSystem";
const QString QOfonoIpMultimediaSystem::Interface::METHOD_REGISTER(QStringLiteral("Register"));
const QString QOfonoIpMultimediaSystem::Interface::METHOD_UNREGISTER(QStringLiteral("Unregister"));
const QString QOfonoIpMultimediaSystem::Interface::PROP_REGISTERED(QStringLiteral("Registered"));
const QString QOfonoIpMultimediaSystem::Interface::PROP_VOICE_CAPABLE(QStringLiteral("VoiceCapable"));
const QString QOfonoIpMultimediaSystem::Interface::PROP_SMS_CAPABLE(QStringLiteral("SmsCapable"));

bool QOfonoIpMultimediaSystem::Interface::syncCall(Interface *iface, const QString &method, QDBusError *outError)
{
    bool ok = false;
    QDBusError error;
    if (iface) {
        QDBusMessage reply(iface->call(method));
        if (reply.type() == QDBusMessage::ErrorMessage) {
            error = QDBusError(reply);
        } else {
            ok = true;
        }
    }
    if (outError) {
        *outError = error;
    }
    return ok;
}

// ==========================================================================
// QOfonoIpMultimediaSystem
// ==========================================================================

#define SUPER QOfonoModemInterface

QOfonoIpMultimediaSystem::QOfonoIpMultimediaSystem(QObject *parent) :
    SUPER(QLatin1String(Interface::NAME), parent)
{
}

QOfonoIpMultimediaSystem::~QOfonoIpMultimediaSystem()
{
}

QDBusAbstractInterface *QOfonoIpMultimediaSystem::createDbusInterface(const QString &path)
{
    return new Interface(path, this);
}

void QOfonoIpMultimediaSystem::propertyChanged(const QString &property, const QVariant &value)
{
    if (property == Interface::PROP_REGISTERED) {
        Q_EMIT registeredChanged();
    } else if (property == Interface::PROP_VOICE_CAPABLE) {
        Q_EMIT voiceCapableChanged();
    } else if (property == Interface::PROP_SMS_CAPABLE) {
        Q_EMIT smsCapableChanged();
    }
    SUPER::propertyChanged(property, value);
}

bool QOfonoIpMultimediaSystem::registered() const
{
    return getBool(Interface::PROP_REGISTERED);
}

bool QOfonoIpMultimediaSystem::voiceCapable() const
{
    return getBool(Interface::PROP_VOICE_CAPABLE);
}

bool QOfonoIpMultimediaSystem::smsCapable() const
{
    return getBool(Interface::PROP_SMS_CAPABLE);
}

void QOfonoIpMultimediaSystem::setRegistered(bool registered)
{
    Interface* iface = qobject_cast<Interface*>(dbusInterface());
    if (iface) {
        if (registered) {
            iface->Register();
        } else {
            iface->Unregister();
        }
    }
}

bool QOfonoIpMultimediaSystem::registerSync(QDBusError *error)
{
    return Interface::syncCall(qobject_cast<Interface*>(dbusInterface()), Interface::METHOD_REGISTER, error);
}

bool QOfonoIpMultimediaSystem::unregisterSync(QDBusError *error)
{
    return Interface::syncCall(qobject_cast<Interface*>(dbusInterface()), Interface::METHOD_UNREGISTER, error);
}

#include "qofonoipmultimediasystem.moc"
