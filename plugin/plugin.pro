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

OTHER_FILES += \
    plugin.json plugins.qmltypes qmldir

qmldir.path = $$[QT_INSTALL_QML]/MeeGo/QOfono
target.path = $$[QT_INSTALL_QML]/MeeGo/QOfono

qmldir.files += qmldir plugins.qmltypes

INSTALLS += target qmldir

qmltypes.commands = qmlplugindump -nonrelocatable MeeGo.QOfono 0.2 > $$PWD/plugins.qmltypes
QMAKE_EXTRA_TARGETS += qmltypes
