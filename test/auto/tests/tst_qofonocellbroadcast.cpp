/*
 * This file is part of ofono-qt
 *
 * Copyright (C) 2012 Jolla
 *
 * Contact: Lorn Potter <lorn.potter@jollamobile.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public License
 * version 2.1 as published by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 *
 */

#include <QtTest/QtTest>

#include "qofonocellbroadcast.h"
#include "ofono_cell_broadcast_interface.h"

class TestCellBroadcast : public QOfonoCellBroadcast
{
public:
    explicit TestCellBroadcast(QObject *parent = 0) :
        QOfonoCellBroadcast(parent)
    {
    }

    OfonoCellBroadcast *createInterface()
    {
        return static_cast<OfonoCellBroadcast *>(
            createDbusInterface(QLatin1String("/test")));
    }
};

class TestQOfonoCellBroadcast : public QObject
{
    Q_OBJECT

private slots:
    void testIncomingBroadcastWithProperties()
    {
        TestCellBroadcast cellBroadcast;
        OfonoCellBroadcast *interface = cellBroadcast.createInterface();
        QSignalSpy spy(&cellBroadcast,
            &QOfonoCellBroadcast::incomingBroadcastWithProperties);
        const QString message = QLatin1String("Test broadcast");
        QVariantMap properties;
        properties.insert(QLatin1String("MessageCode"), 42);

        const int signalIndex = interface->metaObject()->indexOfSignal(
            "IncomingBroadcastWithProperties(QString,QVariantMap)");
        QVERIFY(signalIndex >= 0);
        QVERIFY(interface->metaObject()->method(signalIndex).invoke(
            interface, Qt::DirectConnection,
            Q_ARG(QString, message), Q_ARG(QVariantMap, properties)));

        QCOMPARE(spy.count(), 1);
        const QList<QVariant> arguments = spy.takeFirst();
        QCOMPARE(arguments.at(0).toString(), message);
        QCOMPARE(arguments.at(1).toMap(), properties);
    }
};

QTEST_MAIN(TestQOfonoCellBroadcast)
#include "tst_qofonocellbroadcast.moc"
