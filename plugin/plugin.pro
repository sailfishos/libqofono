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

contains(CONFIG,no-module-prefix) {
    system("sed -i 's/@@ModulePrefix@@//' qmldir")
    system("sed -i 's/@@ModulePrefix@@//' plugins.qmltypes")
    MODULENAME = QOfono
} else {
    system("sed -i 's/@@ModulePrefix@@/MeeGo\./' qmldir")
    system("sed -i 's/@@ModulePrefix@@/MeeGo\./' plugins.qmltypes")
    MODULENAME = MeeGo/QOfono
}

OTHER_FILES += \
    plugin.json plugins.qmltypes qmldir

TARGETPATH = $$[QT_INSTALL_QML]/$$MODULENAME

qmldir.files += qmldir plugins.qmltypes
qmldir.path = $$TARGETPATH
target.path = $$TARGETPATH

INSTALLS += target qmldir

qmltypes.target = qmltypes

contains(CONFIG,no-module-prefix) {
    qmltypes.commands = qmlplugindump -nonrelocatable QOfono 0.2 > $$PWD/plugins.qmltypes
} else {
    qmltypes.commands = qmlplugindump -nonrelocatable MeeGo.QOfono 0.2 \
        |sed 's/MeeGo\.QOfono/@@ModulePrefix@@QOfono/' > $$PWD/plugins.qmltypes
}

QMAKE_EXTRA_TARGETS += qmltypes
