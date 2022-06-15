TEMPLATE = app
QT += qml quick

SOURCES += main.cpp

contains(CONFIG,no-module-prefix) {
    system("sed -i 's/@@ModulePrefix@@//' qml/ofonotest/main.qml")
} else {
    system("sed -i 's/@@ModulePrefix@@/MeeGo\./' qml/ofonotest/main.qml")
}

qml.files += \
    qml/ofonotest/main.qml
qml.path = /opt/examples/libqofono-qt5/qml/ofonotest/main.qml

target.path = /opt/examples/libqofono-qt5/
INSTALLS += target qml
