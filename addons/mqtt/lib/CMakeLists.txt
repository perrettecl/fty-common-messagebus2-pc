
project(fty_messagebus2_mqtt
  VERSION ${PROJECT_VERSION}
  DESCRIPTION "fty messagebus2 mqtt library"
)

##############################################################################################################
find_package(PahoMqttCpp REQUIRED)
##############################################################################################################

etn_target(shared ${PROJECT_NAME} PUBLIC
  INCLUDE_DIRS
    private_include
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
    paho-mqttpp3
    paho-mqtt3as
    paho-mqtt3c
  FLAGS
    "-fmax-errors=2"
)
