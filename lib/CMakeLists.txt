project(fty-common-messagebus2
    VERSION 1.0.0
    DESCRIPTION "fty common messagebus2 library"
)

##############################################################################################################

etn_target(shared ${PROJECT_NAME_UNDERSCORE}
  PUBLIC_INCLUDE_DIR
    public_include
  PUBLIC_HEADERS
    *.hpp
  USES_PUBLIC
    fty_common_messagebus_utils
    fty-messagebus-common
    fty_common_logging
  FLAGS
  "-fmax-errors=1"
)

if(BUILD_AMQP)
  file(GLOB_RECURSE AMQP_SOURCES *Amqp.cpp)
  target_sources(${PROJECT_NAME_UNDERSCORE} PRIVATE ${AMQP_SOURCES})
  target_link_libraries(${PROJECT_NAME_UNDERSCORE}
    PUBLIC
      fty_common_messagebus2_amqp
  )
endif()

if(BUILD_MALAMUTE)
  file(GLOB_RECURSE MALAMUTE_SOURCES *Malamute.cpp)
  target_sources(${PROJECT_NAME_UNDERSCORE} PRIVATE ${MALAMUTE_SOURCES})
  target_link_libraries(${PROJECT_NAME_UNDERSCORE}
    PUBLIC
      fty_common_messagebus2_malamute
  )
endif()

if(BUILD_MQTT)
  file(GLOB_RECURSE MQTT_SOURCES *Mqtt.cpp)
  target_sources(${PROJECT_NAME_UNDERSCORE} PRIVATE ${MQTT_SOURCES})
  target_link_libraries(${PROJECT_NAME_UNDERSCORE}
    PUBLIC
      fty_common_messagebus2_mqtt
  )
endif()

set_target_properties(${PROJECT_NAME_UNDERSCORE} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
