# Test target
etn_test_target(${PROJECT_NAME_UNDERSCORE}
  SOURCES
    src/UniTests.cpp
  USES
    ${PROJECT_NAME_UNDERSCORE}
)

if(BUILD_AMQP)
  target_sources(${PROJECT_NAME_UNDERSCORE}-test PRIVATE src/MsgBusAmqp.cpp)
endif()

if(BUILD_MALAMUTE)
  target_sources(${PROJECT_NAME_UNDERSCORE}-test PRIVATE src/MsgBusMalamute.cpp)
endif()

if(BUILD_MQTT)
  target_sources(${PROJECT_NAME_UNDERSCORE}-test PRIVATE src/MsgBusMqtt.cpp)
endif()
