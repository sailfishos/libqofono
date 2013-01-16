/****************************************************************************
**
** Copyright (C) 2013 Jolla Ltd.
** Contact: lorn.potter@jollamobile.com
**

**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qofonoconnectionmanager.h"
#include "dbus/ofonoconnectionmanager.h"
#include "dbustypes.h"

class QOfonoConnectionManagerPrivate
{
public:
    QOfonoConnectionManagerPrivate();
    QString modemPath;
    OfonoConnectionManager *connman;
    QVariantMap properties;
    QStringList contexts;
};

QOfonoConnectionManagerPrivate::QOfonoConnectionManagerPrivate() :
    modemPath(QString())
   , connman(0)
  ,contexts(QStringList())
{
}

QOfonoConnectionManager::QOfonoConnectionManager(QObject *parent) :
    QObject(parent),
    d_ptr(new QOfonoConnectionManagerPrivate)
{
}

QOfonoConnectionManager::~QOfonoConnectionManager()
{
    delete d_ptr;
}

void QOfonoConnectionManager::setModemPath(const QString &path)
{
    if (!d_ptr->connman) {
        d_ptr->connman = new OfonoConnectionManager("org.ofono", path, QDBusConnection::systemBus(),this);

        if (d_ptr->connman->isValid()) {
            d_ptr->modemPath = path;

            connect(d_ptr->connman,SIGNAL(PropertyChanged(QString,QDBusVariant)),
                    this,SLOT(propertyChanged(QString,QDBusVariant)));
            connect(d_ptr->connman,SIGNAL(ContextAdded(QDBusObjectPath,QVariantMap)),
                    this,SLOT(onContextAdd(QDBusObjectPath,QVariantMap)));
            connect(d_ptr->connman,SIGNAL(ContextRemoved(QDBusObjectPath)),
                    this,SLOT(onContextRemove(QDBusObjectPath)));

            QDBusReply<QVariantMap> reply;
            reply = d_ptr->connman->GetProperties();
            d_ptr->properties = reply.value();

            QArrayOfPathProperties contexts;
            QStringList contextList;

            QDBusReply<QArrayOfPathProperties> reply2 = d_ptr->connman->GetContexts();
            contexts = reply2;
            foreach(OfonoPathProperties context, reply2.value()) {
                contextList.append(context.path.path());
            }
            d_ptr->contexts = contextList;
        }
    }
}

QString QOfonoConnectionManager::modemPath() const
{
    return d_ptr->modemPath;
}

void QOfonoConnectionManager::deactivateAll()
{
    if (!d_ptr->connman)
        return;
    d_ptr->connman->DeactivateAll();
}

void QOfonoConnectionManager::addContext(const QString &type)
{
    if (!d_ptr->connman)
        return;
     QDBusPendingReply<QDBusObjectPath> reply = d_ptr->connman->AddContext(type);
     if (reply.isError())
         qDebug() << Q_FUNC_INFO <<reply.error();
}

void QOfonoConnectionManager::removeContext(const QString &path)
{
    if (!d_ptr->connman)
        return;
    d_ptr->connman->RemoveContext(QDBusObjectPath(path));
    if (d_ptr->contexts.contains(path))
        d_ptr->contexts.removeOne(path);
}

bool QOfonoConnectionManager::attached() const
{
    if (d_ptr->connman)
        return d_ptr->properties["Attached"].value<bool>();
    else
        return false;
}

QString QOfonoConnectionManager::bearer() const
{
    if (d_ptr->connman)
        return d_ptr->properties["Bearer"].value<QString>();
    else
        return QString();
}

bool QOfonoConnectionManager::suspended() const
{
    if (d_ptr->connman)
        return d_ptr->properties["Suspended"].value<bool>();
    else
        return false;
}


bool QOfonoConnectionManager::roamingAllowed() const
{
    if (d_ptr->connman)
        return d_ptr->properties["RoamingAllowed"].value<bool>();
    else
        return false;
}

void QOfonoConnectionManager::setRoamingAllowed(bool b)
{
    if (d_ptr->connman)
        d_ptr->connman->SetProperty("RoamingAllowed",QDBusVariant(b));
}


bool QOfonoConnectionManager::powered() const
{
    if (d_ptr->connman)
        return d_ptr->properties["Powered"].value<bool>();
    else
        return false;
}

void QOfonoConnectionManager::setPowered(bool b)
{
    if (d_ptr->connman)
        d_ptr->connman->SetProperty("Powered",QDBusVariant(b));
}


void QOfonoConnectionManager::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

    if (property == QLatin1String("Attached")) {
        Q_EMIT attachedChanged(value.value<bool>());
    } else if (property == QLatin1String("Bearer")) {
        Q_EMIT bearerChanged(value.value<QString>());
    } else if (property == QLatin1String("Suspended")) {
        Q_EMIT suspendedChanged(value.value<bool>());
    } else if (property == QLatin1String("RoamingAllowed")) {
        Q_EMIT roamingAllowedChanged(value.value<bool>());
    } else if (property == QLatin1String("Powered")) {
        Q_EMIT poweredChanged(value.value<bool>());
    }
}

QStringList QOfonoConnectionManager::contexts()
{
    return d_ptr->contexts;
}

void QOfonoConnectionManager::onContextAdd(const QDBusObjectPath &path, const QVariantMap &propertyMap)
{
    Q_UNUSED(propertyMap);
    if (!d_ptr->contexts.contains(path.path()))
        d_ptr->contexts.append(path.path());
    Q_EMIT contextAdded(path.path());
}

void QOfonoConnectionManager::onContextRemove(const QDBusObjectPath &path)
{
    if (!d_ptr->contexts.contains(path.path()))
        d_ptr->contexts.removeOne(path.path());
    Q_EMIT contextRemoved(path.path());
}
