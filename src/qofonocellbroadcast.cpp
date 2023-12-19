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
#include "qofonocellbroadcast.h"
#include "ofono_cell_broadcast_interface.h"

#define SUPER QOfonoModemInterface

namespace {
    const QString Powered("Powered");
    const QString Topics("Topics");
}

QOfonoCellBroadcast::QOfonoCellBroadcast(QObject *parent)
    : SUPER(OfonoCellBroadcast::staticInterfaceName(), parent)
{
}

QOfonoCellBroadcast::~QOfonoCellBroadcast()
{
}

QDBusAbstractInterface *QOfonoCellBroadcast::createDbusInterface(const QString &path)
{
    OfonoCellBroadcast *iface = new OfonoCellBroadcast(OFONO_SERVICE, path, OFONO_BUS, this);
    connect(iface,
        SIGNAL(IncomingBroadcast(QString,quint16)),
        SIGNAL(incomingBroadcast(QString,quint16)));
    connect(iface,
        SIGNAL(EmergencyBroadcast(QString,QVariantMap)),
        SIGNAL(emergencyBroadcast(QString,QVariantMap)));
    return iface;
}

void QOfonoCellBroadcast::propertyChanged(const QString &property, const QVariant &value)
{
    SUPER::propertyChanged(property, value);
    if (property == Powered) {
        Q_EMIT enabledChanged(value.toBool());
    } else if (property == Topics) {
        Q_EMIT topicsChanged(value.toString());
    }
}

bool QOfonoCellBroadcast::enabled() const
{
    return getBool(Powered);
}

void QOfonoCellBroadcast::setEnabled(bool b)
{
    setProperty(Powered, b);
}

QString QOfonoCellBroadcast::topics() const
{
    return getString(Topics);
}

void QOfonoCellBroadcast::setTopics(const QString &topics)
{
    setProperty(Topics, topics);
}
