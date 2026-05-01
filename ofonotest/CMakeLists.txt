project(ofonotest LANGUAGES CXX)

add_executable(ofonotest
    main.cpp
)

target_link_libraries(ofonotest PRIVATE
    ${QTQUICK_LIB}
    ${QTQML_LIB}
)

install(TARGETS ofonotest
    RUNTIME DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/libqofono-qt${QT_MAJOR_VERSION}/examples
)

install(FILES qml/ofonotest/main.qml
    DESTINATION /usr/share/libqofono-qt${QT_MAJOR_VERSION}/examples/qml/ofonotest/
)
