project(QOfonoQtDeclarative LANGUAGES CXX)

set(PLUGIN_SOURCES
    qofonodeclarativeplugin.cpp
    qofononetworkoperatorlistmodel.cpp
    qofonosimlistmodel.cpp
)

add_library(QOfonoQtDeclarative SHARED ${PLUGIN_SOURCES})

target_include_directories(QOfonoQtDeclarative PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(QOfonoQtDeclarative PRIVATE
    ${QTQML_LIB}
    ${QTDBUS_LIB}
    qofono-qt${QT_MAJOR_VERSION}
)

# Set the output directory for the plugin and copy the qmldir file 
# to the appropriate location for qmlplugindump to find it.
set_target_properties(QOfonoQtDeclarative PROPERTIES
    LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/QOfono
)
configure_file(qmldir ${CMAKE_CURRENT_BINARY_DIR}/QOfono/qmldir COPYONLY)

# qmlplugindump is deprecated in Qt 6, qmltyperegistrar should be used
# by the build system instead using QML_ELEMENT macros to declare the 
# types in the plugin.
if(QT_MAJOR_VERSION STREQUAL "5")
    find_program(QMLPLUGINDUMP_CMD NAMES qmlplugindump-qt5 qmlplugindump)
    
    add_custom_target(qmltypes
        COMMAND ${CMAKE_COMMAND} -E env "QML2_IMPORT_PATH=${CMAKE_CURRENT_BINARY_DIR}" ${QMLPLUGINDUMP_CMD} -nonrelocatable QOfono 0.2 > ${CMAKE_CURRENT_SOURCE_DIR}/plugins.qmltypes
        DEPENDS QOfonoQtDeclarative
        COMMENT "Generating plugins.qmltypes"
    )
endif()

install(TARGETS QOfonoQtDeclarative
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt${QT_MAJOR_VERSION}/qml/QOfono
)

install(FILES qmldir plugins.qmltypes
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/qt${QT_MAJOR_VERSION}/qml/QOfono
)