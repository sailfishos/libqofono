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

#ifndef QOFONOCELLBROADCAST_H
#define QOFONOCELLBROADCAST_H

#include "qofonomodeminterface.h"
#include "qofono_global.h"

//! This class is used to access ofono cell broadcast API
/*!
 * The API is documented in
 * http://git.kernel.org/?p=network/ofono/ofono.git;a=blob_plain;f=doc/cell-broadcast-api.txt
 */
class QOFONOSHARED_EXPORT QOfonoCellBroadcast : public QOfonoModemInterface
{
    Q_OBJECT
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
    Q_PROPERTY(QString topics READ topics WRITE setTopics NOTIFY topicsChanged)

public:
    explicit QOfonoCellBroadcast(QObject *parent = 0);
    ~QOfonoCellBroadcast();

    bool enabled() const;
    void setEnabled(bool b);

    QString topics() const;
    void setTopics(const QString &);

Q_SIGNALS:
    void enabledChanged(bool);
    void topicsChanged(const QString &);
    void incomingBroadcast(const QString &text, quint16 topic);
    void emergencyBroadcast(const QString &text, const QVariantMap &properties);

protected:
    QDBusAbstractInterface *createDbusInterface(const QString &path);
    void propertyChanged(const QString &key, const QVariant &value);
};

#endif // QOFONOCELLBROADCAST_H
