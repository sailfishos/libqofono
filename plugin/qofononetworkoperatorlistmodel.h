/****************************************************************************
**
** Copyright (C) 2014-2019 Jolla Ltd.
** Contact: slava.monich@jolla.com
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

#ifndef QOFONONETWORKOPERATORLISTMODEL_H
#define QOFONONETWORKOPERATORLISTMODEL_H

#include "qofononetworkregistration.h"

class QOfonoNetworkOperatorListModel : public QAbstractListModel {
    Q_OBJECT
    Q_PROPERTY(QString modemPath READ modemPath WRITE setModemPath NOTIFY modemPathChanged)
    Q_PROPERTY(bool valid READ isValid NOTIFY validChanged)
    Q_PROPERTY(int count READ count NOTIFY countChanged)

public:
    enum OperatorRole {
        PathRole = Qt::UserRole + 1,
        NameRole,
        StatusRole,
        MccRole,
        MncRole,
        TechRole,
        InfoRole,
        CountryRole
    };

    explicit QOfonoNetworkOperatorListModel(QObject* parent = NULL);

    QString modemPath() const;
    void setModemPath(const QString &path);
    bool isValid() const;
    int count() const;

    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;

protected:
    QHash<int,QByteArray> roleNames() const;

Q_SIGNALS:
    void modemPathChanged(const QString &value);
    void validChanged(bool value);
    void countChanged(int value);

private Q_SLOTS:
    void onNetworkOperatorsChanged(const QStringList &list);
    void onOperatorNameChanged();
    void onOperatorStatusChanged();
    void onOperatorMccChanged();
    void onOperatorMncChanged();
    void onOperatorTechChanged();
    void onOperatorInfoChanged();
    void onOperatorPathChanged();

private:
    void operatorPropertyChanged(OperatorRole role);
    void operatorPropertyChanged(const QVector<int> &roles);

private:
    QOfonoNetworkRegistration *netreg;
    QStringList operators;
};

#endif // QOFONONETWORKOPERATORLISTMODEL_H
