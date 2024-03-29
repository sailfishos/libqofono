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
#include "qofonohandsfreeaudiomanager.h"
#include "ofono_handsfree_audio_manager_interface.h"

class QOfonoHandsfreeAudioManagerPrivate
{
public:
    QOfonoHandsfreeAudioManagerPrivate();
    QString modemPath;
    OfonoHandsfreeAudioManager *ofonoHandsfreeAudioManager;
    QStringList audioCards;

};

QOfonoHandsfreeAudioManagerPrivate::QOfonoHandsfreeAudioManagerPrivate()
    : modemPath(QString())
    , ofonoHandsfreeAudioManager(0)
    , audioCards(QStringList())
{
}

QOfonoHandsfreeAudioManager::QOfonoHandsfreeAudioManager(QObject *parent)
    : QObject(parent)
    , d_ptr(new QOfonoHandsfreeAudioManagerPrivate)
{
}

QOfonoHandsfreeAudioManager::~QOfonoHandsfreeAudioManager()
{
    delete d_ptr;
}

void QOfonoHandsfreeAudioManager::setModemPath(const QString &path)
{
    if (path == d_ptr->modemPath || path.isEmpty())
        return;

    if (!d_ptr->ofonoHandsfreeAudioManager) {
        if (path != modemPath()) {
            d_ptr->ofonoHandsfreeAudioManager = new OfonoHandsfreeAudioManager(OFONO_SERVICE, path, OFONO_BUS,this);
            if (d_ptr->ofonoHandsfreeAudioManager) {
                d_ptr->modemPath = path;
                QDBusReply<ObjectPathPropertiesList> reply2 = d_ptr->ofonoHandsfreeAudioManager->GetCards();
                foreach(ObjectPathProperties card, reply2.value()) {
                    d_ptr->audioCards << card.path.path();
                    Q_EMIT cardAdded(card.path.path());
                }
                Q_EMIT modemPathChanged(path);
            }
        }
    }
}

QString QOfonoHandsfreeAudioManager::modemPath() const
{
    return d_ptr->modemPath;
}

QStringList QOfonoHandsfreeAudioManager::cards() const
{
    if (d_ptr->ofonoHandsfreeAudioManager) {
        return d_ptr->audioCards;
    }
    return QStringList();
}

void QOfonoHandsfreeAudioManager::registerAgent(const QString &path, CodecTypeFlags codecs)
{
    QByteArray ba;

    if (codecs & CVSD) ba += CVSD;
    if (codecs & mSBC) ba += mSBC;
    d_ptr->ofonoHandsfreeAudioManager->Register(QDBusObjectPath(path), ba);
}

bool QOfonoHandsfreeAudioManager::isValid() const
{
    return d_ptr->ofonoHandsfreeAudioManager->isValid();
}
