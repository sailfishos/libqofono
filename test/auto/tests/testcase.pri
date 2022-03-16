CONFIG += testcase
QT = core testlib dbus
QOFONO_DIR = ../../../src/
INCLUDEPATH += ../lib ../ $$QOFONO_DIR

LIBS += -L$$QOFONO_DIR -lqofono-qt5
target.path = $$[QT_INSTALL_LIBS]/libqofono-qt5/tests

INSTALLS += target
