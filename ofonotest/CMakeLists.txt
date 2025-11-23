project(ofonotest LANGUAGES CXX)

add_executable(ofonotest
    main.cpp
)

target_link_libraries(ofonotest PRIVATE
    Qt5::Quick
    Qt5::Qml
)

set(OFONO_EXAMPLES_PREFIX "/opt/examples/libqofono-qt${QT_MAJOR_VERSION}")

install(TARGETS ofonotest
    RUNTIME DESTINATION ${OFONO_EXAMPLES_PREFIX}
)

install(FILES qml/ofonotest/main.qml
    DESTINATION ${OFONO_EXAMPLES_PREFIX}/qml/ofonotest/main.qml
)
