TARGET=QOfonoQtDeclarative
TEMPLATE = lib
QT += dbus
CONFIG += plugin

QT_VERSION=$$[QT_VERSION]

equals(QT_MAJOR_VERSION, 4):{
    QT += declarative
    PLUGIN_TYPE = declarative
    LIBS += -L../src -lqofono
}

equals(QT_MAJOR_VERSION, 5): {
    QT += qml
    QT -= gui
    LIBS += -L../src -lqofono-qt5
}

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

equals(QT_MAJOR_VERSION, 4): {
    qmldir.path = $$[QT_INSTALL_IMPORTS]/MeeGo/QOfono
    target.path = $$[QT_INSTALL_IMPORTS]/MeeGo/QOfono
}

equals(QT_MAJOR_VERSION, 5): {
    qmldir.path = $$[QT_INSTALL_QML]/MeeGo/QOfono
    target.path = $$[QT_INSTALL_QML]/MeeGo/QOfono
}

qmldir.files += qmldir plugins.qmltypes

INSTALLS += target qmldir

qmltypes.commands = qmlplugindump -nonrelocatable MeeGo.QOfono 0.2 > $$PWD/plugins.qmltypes
QMAKE_EXTRA_TARGETS += qmltypes
