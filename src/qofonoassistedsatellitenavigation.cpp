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
#include "qofonoassistedsatellitenavigation.h"
#include "ofono_assisted_satellite_navigation_interface.h"

class QOfonoAssistedSatelliteNavigationPrivate
{
public:
    QOfonoAssistedSatelliteNavigationPrivate();
    QString modemPath;
    OfonoAssistedSatelliteNavigation *ofonoAssistedSatelliteNav;
};

QOfonoAssistedSatelliteNavigationPrivate::QOfonoAssistedSatelliteNavigationPrivate()
    : modemPath(QString())
    , ofonoAssistedSatelliteNav(0)
{
}

QOfonoAssistedSatelliteNavigation::QOfonoAssistedSatelliteNavigation(QObject *parent)
    : QObject(parent)
    , d_ptr(new QOfonoAssistedSatelliteNavigationPrivate)
{
}

QOfonoAssistedSatelliteNavigation::~QOfonoAssistedSatelliteNavigation()
{
    delete d_ptr;
}


void QOfonoAssistedSatelliteNavigation::setModemPath(const QString &path)
{
    if (path == d_ptr->modemPath ||
            path.isEmpty())
        return;

    if (path != modemPath()) {
        delete d_ptr->ofonoAssistedSatelliteNav;
        d_ptr->ofonoAssistedSatelliteNav = new OfonoAssistedSatelliteNavigation(OFONO_SERVICE, path, OFONO_BUS, this);
        if (d_ptr->ofonoAssistedSatelliteNav->isValid()) {
            d_ptr->modemPath = path;
            Q_EMIT modemPathChanged(path);
        }
    }
}

QString QOfonoAssistedSatelliteNavigation::modemPath() const
{
    return d_ptr->modemPath;
}

void QOfonoAssistedSatelliteNavigation::registerPositioningRequestAgent(const QString &path)
{
    if (d_ptr->ofonoAssistedSatelliteNav) {
        QDBusPendingReply <> reply = d_ptr->ofonoAssistedSatelliteNav->RegisterPositioningRequestAgent(QDBusObjectPath(path));
        if (reply.isError())
            qDebug() << reply.error().message();
    }
}

void QOfonoAssistedSatelliteNavigation::sendPositioningElement(const QString &xmlElement)
{
    if (d_ptr->ofonoAssistedSatelliteNav)
        d_ptr->ofonoAssistedSatelliteNav->SendPositioningElement(xmlElement);
}

void QOfonoAssistedSatelliteNavigation::unregisterPositioningRequestAgent(const QString &path)
{
    if (d_ptr->ofonoAssistedSatelliteNav)
        d_ptr->ofonoAssistedSatelliteNav->UnregisterPositioningRequestAgent(QDBusObjectPath(path));
}

bool QOfonoAssistedSatelliteNavigation::isValid() const
{
    return d_ptr->ofonoAssistedSatelliteNav->isValid();
}
