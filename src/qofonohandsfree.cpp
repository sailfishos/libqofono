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
#include "qofonohandsfree.h"
#include "ofono_handsfree_interface.h"

class QOfonoHandsfreePrivate
{
public:
    QOfonoHandsfreePrivate();
    QString modemPath;
    OfonoHandsfree *ofonoHandsFree;
    QVariantMap properties;

};

QOfonoHandsfreePrivate::QOfonoHandsfreePrivate()
    : modemPath(QString())
    , ofonoHandsFree(0)
{
}

QOfonoHandsfree::QOfonoHandsfree(QObject *parent)
    : QObject(parent)
    , d_ptr(new QOfonoHandsfreePrivate)
{
}

QOfonoHandsfree::~QOfonoHandsfree()
{
    delete d_ptr;
}

void QOfonoHandsfree::setModemPath(const QString &path)
{
    if (path == d_ptr->modemPath
            || path.isEmpty())
        return;

    if (path != modemPath()) {
        d_ptr->properties.clear();
        delete d_ptr->ofonoHandsFree;
        d_ptr->ofonoHandsFree = new OfonoHandsfree(OFONO_SERVICE, path, OFONO_BUS, this);

        d_ptr->modemPath = path;
        connect(d_ptr->ofonoHandsFree, SIGNAL(PropertyChanged(QString,QDBusVariant)),
                this, SLOT(propertyChanged(QString,QDBusVariant)));

        QDBusPendingReply<QVariantMap> reply;
        reply = d_ptr->ofonoHandsFree->GetProperties();
        reply.waitForFinished();
        d_ptr->properties = reply.value();
        Q_EMIT modemPathChanged(path);
    }
}

QString QOfonoHandsfree::modemPath() const
{
    return d_ptr->modemPath;
}


void QOfonoHandsfree::propertyChanged(const QString& property, const QDBusVariant& dbusvalue)
{
    QVariant value = dbusvalue.variant();
    d_ptr->properties.insert(property,value);

    if (property == QLatin1String("VoiceRecognition")) {
        Q_EMIT voiceRecognitionChanged(value.value<bool>());
    } else if (property == QLatin1String("EchoCancelingNoiseReduction")) {
        Q_EMIT echoCancelingNoiseReductionChanged(value.value<bool>());
    }
}

QStringList QOfonoHandsfree::features() const
{
    if (d_ptr->ofonoHandsFree) {
        return d_ptr->properties["Features"].value<QStringList>();
    }
    return QStringList();
}

bool QOfonoHandsfree::inbandRinging() const
{
    if (d_ptr->ofonoHandsFree) {
        return d_ptr->properties["InbandRinging"].value<bool>();
    }
    return false;
}

bool QOfonoHandsfree::voiceRecognition() const
{
    if (d_ptr->ofonoHandsFree) {
        return d_ptr->properties["VoiceRecognition"].value<bool>();
    }
    return false;
}

void QOfonoHandsfree::setVoiceRecognition(bool on)
{
    if (d_ptr->ofonoHandsFree) {
        d_ptr->ofonoHandsFree->SetProperty("VoiceRecognition",QDBusVariant(on));
    }
}

bool QOfonoHandsfree::echoCancelingNoiseReduction() const
{
    if (d_ptr->ofonoHandsFree) {
        return d_ptr->properties["EchoCancelingNoiseReduction"].value<bool>();
    }
    return false;
}

void QOfonoHandsfree::setEchoCancelingNoiseReduction(bool on)
{
    if (d_ptr->ofonoHandsFree) {
        d_ptr->ofonoHandsFree->SetProperty("EchoCancelingNoiseReduction",QDBusVariant(on));
    }
}

uchar QOfonoHandsfree::batteryChargeLevel() const
{
    if (d_ptr->ofonoHandsFree) {
        return d_ptr->properties["BatteryChargeLevel"].value<uchar>();
    }
    return 0;
}

bool QOfonoHandsfree::isValid() const
{
    return d_ptr->ofonoHandsFree->isValid();
}


