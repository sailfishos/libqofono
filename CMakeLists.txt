cmake_minimum_required(VERSION 3.12.0 FATAL_ERROR)

set(LIBQOFONO_VERSION "0.124" CACHE STRING "libqofono version")
set(QT_MAJOR_VERSION 5 CACHE STRING "Qt major version")

project(libqofono
    VERSION ${LIBQOFONO_VERSION}
    DESCRIPTION "Library for accessing the ofono daemon"
    HOMEPAGE_URL "https://github.com/sailfishos/libqofono"
    LANGUAGES CXX
)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

enable_testing()

include(GNUInstallDirs)
include(FeatureSummary)
find_package(Qt5 COMPONENTS Core DBus Qml Quick Test)

set_package_properties(Qt5
    PROPERTIES
    TYPE REQUIRED
    DESCRIPTION "Qt5 libraries"
    URL "https://www.qt.io"
)

feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_subdirectory(src)
add_subdirectory(plugin)
add_subdirectory(test)
add_subdirectory(ofonotest)
