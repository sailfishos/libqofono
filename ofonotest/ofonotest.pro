TEMPLATE = app
QT += qml quick

SOURCES += main.cpp

qml.files += \
    qml/ofonotest/main.qml
qml.path = /opt/examples/libqofono-qt$${QT_MAJOR_VERSION}/qml/ofonotest/main.qml

target.path = /opt/examples/libqofono-qt$${QT_MAJOR_VERSION}/
INSTALLS += target qml
