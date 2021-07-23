project(fty-common-messagebus-evol
    VERSION 1.0.0
    DESCRIPTION "fty common messagebus library evolution"
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
    fty-messagebus-utils
  USES_PUBLIC
    fty-messagebus-common
    fty_common_logging
  FLAGS
    "-fmax-errors=1"
)

# target_compile_definitions(${PROJECT_NAME_UNDERSCORE} PRIVATE BUILD_AMQP)
# target_compile_definitions(${PROJECT_NAME_UNDERSCORE} PRIVATE BUILD_MALAMUTE)
# target_compile_definitions(${PROJECT_NAME_UNDERSCORE} PRIVATE BUILD_MQTT)

if(BUILD_AMQP)
  target_link_libraries(${PROJECT_NAME_UNDERSCORE}
    PRIVATE
      fty-messagebus-amqp
  )
endif()

if(BUILD_MALAMUTE)
  target_link_libraries(${PROJECT_NAME_UNDERSCORE}
    PRIVATE
      fty-messagebus-malamute
  )
endif()

if(BUILD_MQTT)
  target_link_libraries(${PROJECT_NAME_UNDERSCORE}
    PRIVATE
      fty-messagebus-mqtt
  )
endif()
