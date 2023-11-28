#-------------------------------------------------
#
# Project created by QtCreator 2013-01-15T03:55:39
#
#-------------------------------------------------

QT += testlib dbus # xmlpatterns
QT -= gui

LIBS += -lqofono-qt$${QT_MAJOR_VERSION}
TARGET = tst_qofonotest-qt$${QT_MAJOR_VERSION}
target.path = /opt/tests/libqofono-qt$${QT_MAJOR_VERSION}

CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
LIBS += -L../../../src
INCLUDEPATH += ../../../src

SOURCES += tst_qofonotest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
isEmpty(PREFIX) {
  PREFIX=/usr
}

INSTALLS += target

