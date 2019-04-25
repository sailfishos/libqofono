TEMPLATE = subdirs
CONFIG += ordered
SUBDIRS += src \
    plugin \
    test \
    ofonotest

OTHER_FILES += rpm/libqofono-qt5.spec \
               TODO \
               README
