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

#include "qofononetworkoperatorlistmodel.h"
#include "qofono.h"

QOfonoNetworkOperatorListModel::QOfonoNetworkOperatorListModel(QObject *parent) :
    QAbstractListModel(parent)
{
    netreg = new QOfonoNetworkRegistration(this);
    onNetworkOperatorsChanged(netreg->networkOperators());
    connect(netreg,
        SIGNAL(validChanged(bool)),
        SIGNAL(validChanged(bool)));
    connect(netreg,
        SIGNAL(modemPathChanged(QString)),
        SIGNAL(modemPathChanged(QString)));
    connect(netreg,
        SIGNAL(networkOperatorsChanged(QStringList)),
        SLOT(onNetworkOperatorsChanged(QStringList)));
}

QString QOfonoNetworkOperatorListModel::modemPath() const
{
    return netreg->modemPath();
}

void QOfonoNetworkOperatorListModel::setModemPath(const QString &path)
{
    netreg->setModemPath(path);
}

bool QOfonoNetworkOperatorListModel::isValid() const
{
    return netreg->isValid();
}

int QOfonoNetworkOperatorListModel::rowCount(const QModelIndex &) const
{
    return operators.count();
}

int QOfonoNetworkOperatorListModel::count() const
{
    return operators.count();
}

QVariant QOfonoNetworkOperatorListModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row >= 0 && row < operators.count()) {
        QOfonoNetworkOperator* op = netreg->networkOperator(operators[row]);
        if (op) {
            switch (role) {
            case PathRole:    return op->operatorPath();
            case NameRole:    return op->name();
            case StatusRole:  return op->status();
            case MccRole:     return op->mcc();
            case MncRole:     return op->mnc();
            case TechRole:    return op->technologies();
            case InfoRole:    return op->additionalInfo();
            case CountryRole: return QOfono::mobileCountryCodeToAlpha2CountryCode(op->mcc().toInt());
            }
        }
    }
    qWarning() << index << role;
    return QVariant();
}

QHash<int,QByteArray> QOfonoNetworkOperatorListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[PathRole]    = "operatorPath";
    roles[NameRole]    = "name";
    roles[StatusRole]  = "status";
    roles[MccRole]     = "mcc";
    roles[MncRole]     = "mnc";
    roles[TechRole]    = "technologies";
    roles[InfoRole]    = "additionalInfo";
    roles[CountryRole] = "country";
    return roles;
}

void QOfonoNetworkOperatorListModel::operatorPropertyChanged(const QVector<int> &roles)
{
    QOfonoNetworkOperator* op = qobject_cast<QOfonoNetworkOperator*>(sender());
    if (op) {
        const int row = operators.indexOf(op->operatorPath());
        if (row >= 0) {
            QModelIndex modelIndex = index(row);
            Q_EMIT dataChanged(modelIndex, modelIndex, roles);
        }
    }
}

void QOfonoNetworkOperatorListModel::operatorPropertyChanged(OperatorRole role)
{
    const QVector<int> roles(1, role);
    operatorPropertyChanged(roles);
}

void QOfonoNetworkOperatorListModel::onNetworkOperatorsChanged(const QStringList &list)
{
    beginResetModel();
    const int oldCount = operators.count();
    const int n = list.count();
    operators = list;
    for (int i=0; i<n; i++) {
        QOfonoNetworkOperator* op = netreg->networkOperator(list[i]);
        if (op) {
            connect(op, SIGNAL(operatorPathChanged(QString)), SLOT(onOperatorPathChanged()), Qt::UniqueConnection);
            connect(op, SIGNAL(nameChanged(QString)), SLOT(onOperatorNameChanged()), Qt::UniqueConnection);
            connect(op, SIGNAL(statusChanged(QString)), SLOT(onOperatorStatusChanged()), Qt::UniqueConnection);
            connect(op, SIGNAL(mccChanged(QString)), SLOT(onOperatorMccChanged()), Qt::UniqueConnection);
            connect(op, SIGNAL(mncChanged(QString)), SLOT(onOperatorMncChanged()), Qt::UniqueConnection);
            connect(op, SIGNAL(technologiesChanged(QStringList)), SLOT(onOperatorTechChanged()), Qt::UniqueConnection);
            connect(op, SIGNAL(additionalInfoChanged(QString)), SLOT(onOperatorInfoChanged()), Qt::UniqueConnection);
        }
    }
    if (oldCount != n) {
        Q_EMIT countChanged(n);
    }
    endResetModel();
}

void QOfonoNetworkOperatorListModel::onOperatorPathChanged()
{
    operatorPropertyChanged(PathRole);
}

void QOfonoNetworkOperatorListModel::onOperatorNameChanged()
{
    operatorPropertyChanged(NameRole);
}

void QOfonoNetworkOperatorListModel::onOperatorStatusChanged()
{
    operatorPropertyChanged(StatusRole);
}

void QOfonoNetworkOperatorListModel::onOperatorMccChanged()
{
    QVector<int> roles;
    roles.reserve(2);
    roles.append(MccRole);
    roles.append(CountryRole);
    operatorPropertyChanged(roles);
}

void QOfonoNetworkOperatorListModel::onOperatorMncChanged()
{
    operatorPropertyChanged(MncRole);
}

void QOfonoNetworkOperatorListModel::onOperatorTechChanged()
{
    operatorPropertyChanged(TechRole);
}

void QOfonoNetworkOperatorListModel::onOperatorInfoChanged()
{
    operatorPropertyChanged(InfoRole);
}
