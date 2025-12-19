project(ofonotest LANGUAGES CXX)

add_executable(ofonotest
    main.cpp
)

target_link_libraries(ofonotest PRIVATE
    ${QTQUICK_LIB}
    ${QTQML_LIB}
)

set(OFONO_EXAMPLES_PREFIX "/opt/examples/libqofono-qt${QT_MAJOR_VERSION}")

install(TARGETS ofonotest
    RUNTIME DESTINATION ${OFONO_EXAMPLES_PREFIX}
)

install(FILES qml/ofonotest/main.qml
    DESTINATION ${OFONO_EXAMPLES_PREFIX}/qml/ofonotest/main.qml
)
