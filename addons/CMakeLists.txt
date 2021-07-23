# Addons implementation.

if(BUILD_MQTT)
  add_subdirectory(mqtt)
endif()

if(BUILD_AMQP)
  add_subdirectory(amqp)
endif()

if(BUILD_MALAMUTE)
  add_subdirectory(malamute)
endif()
