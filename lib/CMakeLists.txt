project(fty-common-messagebus-evol
    VERSION 1.0.0
    DESCRIPTION "fty common messagebus2 library"
)

##############################################################################################################

etn_target(shared ${PROJECT_NAME_UNDERSCORE}
  PUBLIC_INCLUDE_DIR
    public_include
  PUBLIC_HEADERS
    *.hpp
  SOURCES
    src/*.cpp
  USES_PRIVATE
    # fty-messagebus-utils
  USES_PUBLIC
    fty-messagebus-utils
    fty-messagebus-common
    fty_common_logging
  FLAGS
  "-fmax-errors=1"
)

if(BUILD_AMQP)
  target_link_libraries(${PROJECT_NAME_UNDERSCORE}
    PUBLIC
      fty_messagebus2_amqp
  )
endif()

if(BUILD_MALAMUTE)
  target_link_libraries(${PROJECT_NAME_UNDERSCORE}
    PUBLIC
      fty_messagebus2_malamute
  )
endif()

if(BUILD_MQTT)
  target_link_libraries(${PROJECT_NAME_UNDERSCORE}
    PUBLIC
      fty_messagebus2_mqtt
  )
endif()
