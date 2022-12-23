TARGET=QOfonoQtDeclarative
TEMPLATE = lib
QT += dbus
CONFIG += plugin

QT_VERSION=$$[QT_VERSION]

QT += qml
QT -= gui
LIBS += -L../src -lqofono-qt5

SOURCES =  \ 
    qofonodeclarativeplugin.cpp \
    qofononetworkoperatorlistmodel.cpp \
    qofonosimlistmodel.cpp

HEADERS = \
    qofonodeclarativeplugin.h \
    qofononetworkoperatorlistmodel.h \
    qofonosimlistmodel.h

INCLUDEPATH += ../src

MODULENAME = QOfono

OTHER_FILES += \
    plugin.json plugins.qmltypes qmldir

TARGETPATH = $$[QT_INSTALL_QML]/$$MODULENAME

qmldir.files += qmldir plugins.qmltypes
qmldir.path = $$TARGETPATH
target.path = $$TARGETPATH

INSTALLS += target qmldir

qmltypes.target = qmltypes
qmltypes.commands = qmlplugindump -nonrelocatable QOfono 0.2 > $$PWD/plugins.qmltypes

QMAKE_EXTRA_TARGETS += qmltypes
