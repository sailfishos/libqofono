#-------------------------------------------------
#
# Project created by QtCreator 2013-01-15T03:55:39
#
#-------------------------------------------------

QT += testlib dbus # xmlpatterns
QT -= gui

LIBS += -lqofono-qt5
TARGET = tst_qofonotest-qt5
target.path = /opt/tests/libqofono-qt5

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

