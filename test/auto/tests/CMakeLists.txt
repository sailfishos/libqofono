set(TEST_SRCS
    tst_qofonomanager.cpp
    tst_qofonomodem.cpp
    tst_qofonoconnman.cpp
    tst_qofonoconnmancontext.cpp
    tst_qofononetworkoperator.cpp
    tst_qofonophonebook.cpp
    tst_qofononetworkregistration.cpp
    tst_qofonosimmanager.cpp
    tst_qofonomessagewaiting.cpp
    tst_qofonomessagemanager.cpp
    tst_qofonoradiosettings.cpp
    tst_qofonocallbarring.cpp
    tst_qofonocallforwarding.cpp
    tst_qofonocallmeter.cpp
    tst_qofonocallsettings.cpp
    tst_qofonosupplementaryservices.cpp
    tst_qofonovoicecallmanager.cpp
    tst_qofonovoicecall.cpp
    tst_qofonocallvolume.cpp
    tst_qofonomultipartycall.cpp
    tst_qofonocellbroadcast.cpp
)

foreach(SRC ${TEST_SRCS})
    get_filename_component(TEST_NAME ${SRC} NAME_WE)
    add_executable(${TEST_NAME} ${SRC})
    target_include_directories(${TEST_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/../lib
        ${CMAKE_CURRENT_SOURCE_DIR}/..
        ${CMAKE_SOURCE_DIR}/src
    )
    target_link_libraries(${TEST_NAME} PRIVATE Qt5::Test Qt5::Core Qt5::DBus qofono-qt${QT_MAJOR_VERSION})
    add_test(NAME ${TEST_NAME} COMMAND ${TEST_NAME})

    install(TARGETS ${TEST_NAME}
        RUNTIME DESTINATION /opt/tests/libqofono-qt${QT_MAJOR_VERSION}
    )
endforeach()

install(FILES runtest.sh tests.xml
    DESTINATION /opt/tests/libqofono-qt${QT_MAJOR_VERSION}
)
