project(tst_qofonotest-qt${QT_MAJOR_VERSION} LANGUAGES CXX)

add_executable(tst_qofonotest-qt${QT_MAJOR_VERSION}
    tst_qofonotest.cpp
)

target_include_directories(tst_qofonotest-qt${QT_MAJOR_VERSION} PRIVATE
    ${CMAKE_SOURCE_DIR}/src
)

target_link_libraries(tst_qofonotest-qt${QT_MAJOR_VERSION} PRIVATE
    ${QTTEST_LIB}
    ${QTDBUS_LIB}
    qofono-qt${QT_MAJOR_VERSION}
)

install(TARGETS tst_qofonotest-qt${QT_MAJOR_VERSION}
    RUNTIME DESTINATION /opt/tests/libqofono-qt${QT_MAJOR_VERSION}
)
