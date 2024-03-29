/****************************************************************************
**
** Copyright (C) 2014-2022 Jolla Ltd.
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

#ifndef QOFONOUTILS_P_H
#define QOFONOUTILS_P_H

#include "qofono_global.h"

#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
#  define QOfonoSkipEmptyParts Qt::SkipEmptyParts
#else
#  define QOfonoSkipEmptyParts QString::SkipEmptyParts
#endif

namespace qofono {

/* Carefully compare variant values, handling invalid and null values properly.
 * Intentionally not using isNull() as its semantic is not optimal for our use
 * case. E.g.:
 *      QVariant(QString("")).isNull() != QVariant(QString()).isNull()
 */
inline bool safeVariantEq(const QVariant &v1, const QVariant &v2)
{
    if (!v1.isValid() && !v2.isValid())
        return true;
    if (!v1.isValid() && v2 == QVariant(v2.type()))
        return true;
    if (!v2.isValid() && v1 == QVariant(v1.type()))
        return true;
    return v1 == v2;
}

inline bool isTimeout(const QDBusError &error)
{
    switch (error.type()) {
    case QDBusError::NoReply:
    case QDBusError::Timeout:
    case QDBusError::TimedOut:
        return true;
    default:
        return false;
    }
}

}

#endif // QOFONOUTILS_P_H
