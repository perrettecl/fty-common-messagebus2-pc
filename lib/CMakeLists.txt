project(fty-common-messagebus-evol
    VERSION 1.0.0
    DESCRIPTION "fty common messagebus library evolution"
)

##############################################################################################################
etn_target(shared ${PROJECT_NAME_UNDERSCORE}
  PUBLIC_INCLUDE_DIR
    public_include
  PUBLIC_HEADERS
    public_include
  SOURCES
    src/*.cpp
  USES_PRIVATE
    fty-messagebus-utils
    fty-messagebus-mlm
    fty-messagebus-mqtt
  USES_PUBLIC
    fty-messagebus-common
    fty_common_logging
    czmq
    mlm
    pthread
    paho-mqttpp3
    paho-mqtt3as
    paho-mqtt3c
)
