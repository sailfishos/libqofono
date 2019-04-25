TEMPLATE = subdirs
SUBDIRS += src plugin test ofonotest
OTHER_FILES += \
  rpm/libqofono-qt5.spec \
  TODO \
  README

src.target = src-target
plugin.depends = src-target
test.depends = src-target
ofonotest.depends = src-target
