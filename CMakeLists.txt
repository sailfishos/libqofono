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

find_package(Qt${QT_MAJOR_VERSION} COMPONENTS Core DBus Qml Quick Test)
set_package_properties(Qt${QT_MAJOR_VERSION}
    PROPERTIES
    TYPE REQUIRED
    DESCRIPTION "Qt${QT_MAJOR_VERSION} libraries"
    URL "https://www.qt.io"
)
set(QTCORE_LIB Qt${QT_MAJOR_VERSION}::Core)
set(QTDBUS_LIB Qt${QT_MAJOR_VERSION}::DBus)
set(QTQML_LIB Qt${QT_MAJOR_VERSION}::Qml)
set(QTQUICK_LIB Qt${QT_MAJOR_VERSION}::Quick)
set(QTTEST_LIB Qt${QT_MAJOR_VERSION}::Test)

feature_summary(WHAT REQUIRED_PACKAGES_NOT_FOUND FATAL_ON_MISSING_REQUIRED_PACKAGES)

add_subdirectory(src)
add_subdirectory(plugin)
add_subdirectory(test)
add_subdirectory(ofonotest)
