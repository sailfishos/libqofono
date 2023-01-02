TEMPLATE = app
QT += qml quick

SOURCES += main.cpp

qml.files += \
    qml/ofonotest/main.qml
qml.path = /opt/examples/libqofono-qt5/qml/ofonotest

target.path = /opt/examples/libqofono-qt5/
INSTALLS += target qml
