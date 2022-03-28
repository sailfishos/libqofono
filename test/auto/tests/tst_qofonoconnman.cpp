/*
 * This file is part of ofono-qt
 *
 * Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
 *
 * Contact: Alexander Kanavin <alex.kanavin@gmail.com>
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

#include "qofonoconnectioncontext.h"
#include "qofonoconnectionmanager.h"

class TestOfonoConnMan : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase()
    {
        m = new QOfonoConnectionManager(this);
        m->setModemPath("/phonesim");

        QTRY_VERIFY(m->isValid());
        QTRY_VERIFY(m->powered());
    }

    void testOfonoConnMan()
    {
        m->setRoamingAllowed(true);
        QTRY_COMPARE(m->roamingAllowed(), true);

        QSignalSpy roam(m,SIGNAL(roamingAllowedChanged(bool)));
        QSignalSpy pow(m, SIGNAL(poweredChanged(bool)));

        m->setPowered(false);
        QTRY_COMPARE(pow.count(), 1);
        QCOMPARE(pow.takeFirst().at(0).toBool(), false);
        QCOMPARE(m->powered(), false);
        m->setPowered(true);
        QTRY_COMPARE(pow.count(), 1);
        QCOMPARE(pow.takeFirst().at(0).toBool(), true);
        QCOMPARE(m->powered(), true);

        m->setRoamingAllowed(false);
        QTRY_COMPARE(roam.count(), 1);
        QCOMPARE(roam.takeFirst().at(0).toBool(), false);
        QCOMPARE(m->roamingAllowed(), false);
        m->setRoamingAllowed(true);
        QTRY_COMPARE(roam.count(), 1);
        QCOMPARE(roam.takeFirst().at(0).toBool(), true);
        QCOMPARE(m->roamingAllowed(), true);

        QCOMPARE(m->powered(),true);
        QCOMPARE(m->attached(),true);
        QCOMPARE(m->suspended(),false);
        QCOMPARE(m->roamingAllowed(),true);

        QOfonoConnectionContext* context = new QOfonoConnectionContext(this);
        context->setContextPath(m->contexts().first());

        context->setActive(true);
        QTRY_VERIFY(context->active());

        m->deactivateAll();
        QTRY_VERIFY(!context->active());
    }

    void cleanupTestCase()
    {
    }

private:
    QOfonoConnectionManager *m;
};

QTEST_MAIN(TestOfonoConnMan)
#include "tst_qofonoconnman.moc"
