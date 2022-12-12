CONFIG += testcase
QT = core testlib dbus
QOFONO_DIR = ../../../src/
INCLUDEPATH += ../lib ../ $$QOFONO_DIR

LIBS += -L$$QOFONO_DIR -lqofono-qt$${QT_MAJOR_VERSION}
target.path = /opt/tests/libqofono-qt$${QT_MAJOR_VERSION}

INSTALLS += target
