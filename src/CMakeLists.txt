add_library(
    qofono-qt${QT_MAJOR_VERSION} SHARED 
    qofono.cpp
    qofonodbustypes.cpp
    qofonoobject.cpp
    qofonomanager.cpp
    qofonomodem.cpp
    qofonomodeminterface.cpp
    qofonomodeminterface2.cpp
    qofonoassistedsatellitenavigation.cpp
    qofonoconnectionmanager.cpp
    qofonoconnectioncontext.cpp
    qofononetworkregistration.cpp
    qofononetworkoperator.cpp
    qofonocellbroadcast.cpp
    qofonocallbarring.cpp
    qofonocallforwarding.cpp
    qofonocallmeter.cpp
    qofonocallsettings.cpp
    qofonocallvolume.cpp
    qofonohandsfree.cpp
    qofonohandsfreeaudioagent.cpp
    qofonohandsfreeaudiocard.cpp
    qofonohandsfreeaudiomanager.cpp
    qofonoipmultimediasystem.cpp
    qofonolocationreporting.cpp
    qofonomessage.cpp
    qofonomessagemanager.cpp
    qofonomessagewaiting.cpp
    qofonosimmanager.cpp
    qofonosimwatcher.cpp
    qofonophonebook.cpp
    qofonopositioningrequestagent.cpp
    qofonosmartmessaging.cpp
    qofonosmartmessagingagent.cpp
    qofonovoicecall.cpp
    qofonovoicecallmanager.cpp
    qofonoradiosettings.cpp
    qofonosupplementaryservices.cpp
    qofonotexttelephony.cpp
)

set(PUBLIC_HEADER_FILES
    qofono.h
    qofono_global.h
    qofonoobject.h
    qofonomanager.h
    dbustypes.h
    qofonomodem.h
    qofonomodeminterface.h
    qofonomodeminterface2.h
    qofonoassistedsatellitenavigation.h
    qofonoconnectionmanager.h
    qofonoconnectioncontext.h
    qofononetworkregistration.h
    qofononetworkoperator.h
    qofonocellbroadcast.h
    qofonocallbarring.h
    qofonocallforwarding.h
    qofonoipmultimediasystem.h
    qofonolocationreporting.h
    qofonocallmeter.h
    qofonocallsettings.h
    qofonocallvolume.h
    qofonohandsfree.h
    qofonohandsfreeaudioagent.h
    qofonohandsfreeaudiocard.h
    qofonohandsfreeaudiomanager.h
    qofonomessage.h
    qofonomessagemanager.h
    qofonomessagewaiting.h
    qofonosimmanager.h
    qofonosimwatcher.h
    qofonophonebook.h
    qofonopositioningrequestagent.h
    qofonosmartmessaging.h
    qofonosmartmessagingagent.h
    qofonovoicecall.h
    qofonovoicecallmanager.h
    qofonoradiosettings.h
    qofonosupplementaryservices.h
    qofonotexttelephony.h
)

set_target_properties(
    qofono-qt${QT_MAJOR_VERSION}
    PROPERTIES
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR}
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)


target_compile_definitions(qofono-qt${QT_MAJOR_VERSION} PRIVATE QOFONO_LIBRARY)

target_compile_options(qofono-qt${QT_MAJOR_VERSION} PRIVATE -O2 -Wno-unused-parameter)

target_include_directories(qofono-qt${QT_MAJOR_VERSION} PRIVATE ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(qofono-qt${QT_MAJOR_VERSION}
    PRIVATE ${QTCORE_LIB} ${QTDBUS_LIB}
)

### DBus interfaces and adaptors ###

set(_dbus_ifaces "")
set(_xml_files "")

function(ofono_dbus FILENAME CLASSNAME)
    set(_path "${CMAKE_CURRENT_SOURCE_DIR}/dbus/${FILENAME}")
    string(REPLACE ".xml" "_interface" _interface "${FILENAME}")
    cmake_parse_arguments(_ofo "" "INCLUDE" "" "${ARGN}")
    
    if(NOT EXISTS ${_path})
        message(FATAL_ERROR "DBus-XML source ${_path} does not exist")
    endif()

    set_source_files_properties(${_path} PROPERTIES CLASSNAME ${CLASSNAME})
    
    if(_ofo_INCLUDE)
        set_source_files_properties(${_path} PROPERTIES INCLUDE ${_ofo_INCLUDE})
    endif()

    list(APPEND _xml_files ${_path})
    set(_xml_files ${_xml_files} PARENT_SCOPE)
    if (QT_MAJOR_VERSION STREQUAL "6")
        qt6_add_dbus_interface(_dbus_ifaces ${_path} ${_interface})
    else()
        qt5_add_dbus_interface(_dbus_ifaces ${_path} ${_interface})
    endif()
    set(_dbus_ifaces ${_dbus_ifaces} PARENT_SCOPE)
endfunction()

ofono_dbus(ofono_assisted_satellite_navigation.xml OfonoAssistedSatelliteNavigation)
ofono_dbus(ofono_call_barring.xml OfonoCallBarring)
ofono_dbus(ofono_call_forwarding.xml OfonoCallForwarding)
ofono_dbus(ofono_callmeter.xml OfonoCallMeter)
ofono_dbus(ofono_call_settings.xml OfonoCallSettings)
ofono_dbus(ofono_callvolume.xml OfonoCallVolume)
ofono_dbus(ofono_cell_broadcast.xml OfonoCellBroadcast)
ofono_dbus(ofono_handsfree.xml OfonoHandsfree)
ofono_dbus(ofono_handsfree_audio_agent.xml OfonoHandsfreeAudioAgent)
ofono_dbus(ofono_handsfree_audio_card.xml OfonoHandsfreeAudioCard)
ofono_dbus(ofono_handsfree_audio_manager.xml OfonoHandsfreeAudioManager INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/dbustypes.h)
ofono_dbus(ofono_connection_context.xml OfonoConnectionContext)
ofono_dbus(ofono_connection_manager.xml OfonoConnectionManager INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/dbustypes.h)
ofono_dbus(ofono_location_reporting.xml OfonoLocationReporting)
ofono_dbus(ofono_manager.xml OfonoManager INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/dbustypes.h)
ofono_dbus(ofono_message.xml OfonoMessage)
ofono_dbus(ofono_message_manager.xml OfonoMessageManager INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/dbustypes.h)
ofono_dbus(ofono_messagewaiting.xml OfonoMessageWaiting)
ofono_dbus(ofono_modem.xml OfonoModem)
ofono_dbus(ofono_network_operator.xml OfonoNetworkOperator)
ofono_dbus(ofono_network_registration.xml OfonoNetworkRegistration INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/dbustypes.h)
ofono_dbus(ofono_phonebook.xml OfonoPhonebook)
ofono_dbus(ofono_radio_settings.xml OfonoRadioSettings)
ofono_dbus(ofono_simmanager.xml OfonoSimManager)
ofono_dbus(ofono_smart_messaging.xml OfonoSmartMessaging)
ofono_dbus(ofono_supplementary_services.xml OfonoSupplementaryServices)
ofono_dbus(ofono_text_telephony.xml OfonoTextTelephony)
ofono_dbus(ofono_voice_call.xml OfonoVoiceCall)
ofono_dbus(ofono_voicecallmanager.xml OfonoVoiceCallManager INCLUDE ${CMAKE_CURRENT_SOURCE_DIR}/dbustypes.h)

set_source_files_properties(${_dbus_ifaces} PROPERTIES SKIP_AUTOMOC ON)
target_sources(qofono-qt${QT_MAJOR_VERSION} PRIVATE ${_dbus_ifaces})

# FIXME: qdbusxml2cpp doesn't use QMetaObject::invokeMethod(parent(), ...) to emit signals, when using the qt_add_dbus_adaptor generated adaptors because it is invoked with a parent class.
# Might be useful to set a specific parent class that implements all the methods to emit signals.
# For now, we manually invoke qdbusxml2cpp to generate the adaptors without a parent class.
#qt_add_dbus_adaptor(
#    _adaptor_files 
#    ${CMAKE_CURRENT_SOURCE_DIR}/dbus/ofono_positioning_request_agent.xml
#    ${CMAKE_CURRENT_SOURCE_DIR}/qofonopositioningrequestagent.h
#    QOfonoPositioningRequestAgent
#    ofono_positioning_request_agent_adaptor
#    QOfonoPositioningRequestAgentAdaptor
#)
#qt_add_dbus_adaptor(
#    _adaptor_files 
#    ${CMAKE_CURRENT_SOURCE_DIR}/dbus/ofono_smart_messaging_agent.xml
#    ${CMAKE_CURRENT_SOURCE_DIR}/qofonosmartmessagingagent.h
#    QOfonoSmartMessagingAgent
#    ofono_smart_messaging_agent_adaptor
#    QOfonoSmartMessagingAgentAdaptor
#)

# Generate adaptor sources/headers into the CMake binary dir using qdbusxml2cpp.

# List of generated adaptor files
set(_adaptor_files
    ${CMAKE_CURRENT_BINARY_DIR}/ofono_positioning_request_agent_adaptor.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/ofono_positioning_request_agent_adaptor.h
    ${CMAKE_CURRENT_BINARY_DIR}/ofono_smart_messaging_agent_adaptor.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/ofono_smart_messaging_agent_adaptor.h
)

if(QT_MAJOR_VERSION STREQUAL "6")
    # Generate ofono_positioning_request_agent_adaptor.{cpp,h}
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ofono_positioning_request_agent_adaptor.cpp ${CMAKE_CURRENT_BINARY_DIR}/ofono_positioning_request_agent_adaptor.h
        COMMAND Qt6::qdbusxml2cpp -a ${CMAKE_CURRENT_BINARY_DIR}/ofono_positioning_request_agent_adaptor -N -c QOfonoPositioningRequestAgentAdaptor ${CMAKE_CURRENT_SOURCE_DIR}/dbus/ofono_positioning_request_agent.xml
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dbus/ofono_positioning_request_agent.xml
        COMMENT "Generating QDBus adaptor for ofono_positioning_request_agent"
        VERBATIM
    )

    # Generate ofono_smart_messaging_agent_adaptor.{cpp,h}
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ofono_smart_messaging_agent_adaptor.cpp ${CMAKE_CURRENT_BINARY_DIR}/ofono_smart_messaging_agent_adaptor.h
        COMMAND Qt6::qdbusxml2cpp -a ${CMAKE_CURRENT_BINARY_DIR}/ofono_smart_messaging_agent_adaptor -N -c QOfonoSmartMessagingAgentAdaptor ${CMAKE_CURRENT_SOURCE_DIR}/dbus/ofono_smart_messaging_agent.xml 
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dbus/ofono_smart_messaging_agent.xml
        COMMENT "Generating QDBus adaptor for ofono_smart_messaging_agent"
        VERBATIM
    )
else()
    # Generate ofono_positioning_request_agent_adaptor.{cpp,h}
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ofono_positioning_request_agent_adaptor.cpp ${CMAKE_CURRENT_BINARY_DIR}/ofono_positioning_request_agent_adaptor.h
        COMMAND Qt5::qdbusxml2cpp -a ${CMAKE_CURRENT_BINARY_DIR}/ofono_positioning_request_agent_adaptor -N -c QOfonoPositioningRequestAgentAdaptor ${CMAKE_CURRENT_SOURCE_DIR}/dbus/ofono_positioning_request_agent.xml
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dbus/ofono_positioning_request_agent.xml
        COMMENT "Generating QDBus adaptor for ofono_positioning_request_agent"
        VERBATIM
    )

    # Generate ofono_smart_messaging_agent_adaptor.{cpp,h}
    add_custom_command(
        OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/ofono_smart_messaging_agent_adaptor.cpp ${CMAKE_CURRENT_BINARY_DIR}/ofono_smart_messaging_agent_adaptor.h
        COMMAND Qt5::qdbusxml2cpp -a ${CMAKE_CURRENT_BINARY_DIR}/ofono_smart_messaging_agent_adaptor -N -c QOfonoSmartMessagingAgentAdaptor ${CMAKE_CURRENT_SOURCE_DIR}/dbus/ofono_smart_messaging_agent.xml 
        DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/dbus/ofono_smart_messaging_agent.xml
        COMMENT "Generating QDBus adaptor for ofono_smart_messaging_agent"
        VERBATIM
    )
endif()

target_sources(qofono-qt${QT_MAJOR_VERSION} PRIVATE ${_adaptor_files})

LIST(APPEND _xml_files 
    ${CMAKE_CURRENT_SOURCE_DIR}/dbus/ofono_audio_settings.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/dbus/ofono_push_notification.xml
    ${CMAKE_CURRENT_SOURCE_DIR}/dbus/ofono_smart_messaging_agent.xml
)

LIST(REMOVE_ITEM _xml_files
    ${CMAKE_CURRENT_SOURCE_DIR}/dbus/ofono_location_reporting.xml
)

### INSTALL ###

# Library
install(TARGETS qofono-qt${QT_MAJOR_VERSION}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

# Public headers
install(FILES ${PUBLIC_HEADER_FILES}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qofono-qt${QT_MAJOR_VERSION}
)

# DBus XML files
install(FILES ${_xml_files}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/qofono-qt${QT_MAJOR_VERSION}/dbus
)

# pkg-config file (qmake create_pc)
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/qofono-qt.pc.in ${CMAKE_CURRENT_BINARY_DIR}/qofono-qt${QT_MAJOR_VERSION}.pc @ONLY)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/qofono-qt${QT_MAJOR_VERSION}.pc
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/pkgconfig
)
