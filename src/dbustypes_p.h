/****************************************************************************
**
** Copyright (C) 2020 Jolla Ltd.
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

#ifndef DBUSTYPES_PRIVATE_H
#define DBUSTYPES_PRIVATE_H

#include "dbustypes.h"

namespace QOfonoDbus {
    extern const QString Q_DECL_HIDDEN Service;
}

#define OFONO_SERVICE (QOfonoDbus::Service)
#define OFONO_BUS (QDBusConnection::systemBus())

#endif // DBUSTYPES_PRIVATE_H
