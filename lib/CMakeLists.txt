project(fty-common-messagebus-evol
    VERSION 1.0.0
    DESCRIPTION "Common messagebus interface"
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
    fty-messagebus-mlm
    fty-messagebus-mqtt
    jsoncpp
  USES_PUBLIC
    fty_common_logging
    fty-messagebus-common
  USES
    cxxtools
    czmq
    fty_common_mlm
    mlm
    pthread
    paho-mqttpp3
    paho-mqtt3as
    jsoncpp
  FLAGS
      "-fmax-errors=5"
)
