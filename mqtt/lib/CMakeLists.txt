
project(fty-messagebus-mqtt
  VERSION ${PROJECT_VERSION}
  DESCRIPTION "fty messagebus mqtt library"
)

##############################################################################################################
find_package(PahoMqttCpp REQUIRED)
##############################################################################################################

etn_target(static ${PROJECT_NAME}
  INCLUDE_DIRS
    private_include
  PUBLIC_INCLUDE_DIR
    public_include
  PUBLIC_HEADERS
    *.hpp
  SOURCES
    src/*.cpp
  USES_PUBLIC
    fty-messagebus-utils
    fty-messagebus-common
    fty_common_logging
    pthread
    # paho-mqttpp3
    # paho-mqtt3as
    # paho-mqtt3c
  FLAGS
    "-fmax-errors=1"
)
