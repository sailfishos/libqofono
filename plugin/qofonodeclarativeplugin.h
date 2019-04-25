/****************************************************************************
**
** Copyright (C) 2013-2014 Jolla Ltd.
** Contact: lorn.potter@jollamobile.com
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

#ifndef QOFONODECLARATIVEPLUGIN_H
#define QOFONODECLARATIVEPLUGIN_H

#include "qofono_global.h"

#include <QtQml/qqmlextensionplugin.h>
#include <QtQml/qqml.h>
class QOFONOSHARED_EXPORT QOfonoDeclarativePlugin : public QQmlExtensionPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "org.qt-project.Qt.QQmlExtensionInterface" FILE "plugin.json")
public:
    void registerTypes(const char *uri);
    static void registerTypes(const char *uri, int major, int minor);
};

#endif // QOFONODECLARATIVEPLUGIN_H
