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
    target_link_libraries(${TEST_NAME} PRIVATE ${QTTEST_LIB} ${QTCORE_LIB} ${QTDBUS_LIB} qofono-qt${QT_MAJOR_VERSION})
    add_test(NAME ${TEST_NAME} COMMAND ${DBUS_RUNNER}
        --bus-type=both
        --dbus-config=${CMAKE_CURRENT_SOURCE_DIR}/session.conf
        --task ${CMAKE_CURRENT_BINARY_DIR}/testenv.sh
        -p enable
        -p online
        -p ${CMAKE_CURRENT_BINARY_DIR}/${TEST_NAME})
    set_tests_properties(${TEST_NAME} PROPERTIES TIMEOUT 30)

    install(TARGETS ${TEST_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/libqofono-qt${QT_MAJOR_VERSION}/tests
    )
endforeach()

configure_file(tests.xml.in ${CMAKE_CURRENT_BINARY_DIR}/tests.xml)
configure_file(testenv.sh.in ${CMAKE_CURRENT_BINARY_DIR}/testenv.sh)
configure_file(runtest.sh.in ${CMAKE_CURRENT_BINARY_DIR}/runtest.sh @ONLY)

install(PROGRAMS ${CMAKE_CURRENT_BINARY_DIR}/testenv.sh
        ${CMAKE_CURRENT_BINARY_DIR}/runtest.sh
    DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/libqofono-qt${QT_MAJOR_VERSION}/tests
)

install(FILES phonesim.conf session.conf ${CMAKE_CURRENT_BINARY_DIR}/tests.xml
    DESTINATION ${CMAKE_INSTALL_LIBEXECDIR}/libqofono-qt${QT_MAJOR_VERSION}/tests
)
