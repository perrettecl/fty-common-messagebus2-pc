
project(fty-messagebus-mqtt
  VERSION ${PROJECT_VERSION}
  DESCRIPTION "Common messagebus mqtt implementation"
)

##############################################################################################################
find_package(PahoMqttCpp REQUIRED)
find_package(jsoncpp REQUIRED)
find_package(nlohmann_json 3.2.0 REQUIRED)
##############################################################################################################


##############################################################################################################


etn_target(shared ${PROJECT_NAME}
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
    paho-mqttpp3
    paho-mqtt3as
    paho-mqtt3c
    nlohmann_json::nlohmann_json
  FLAGS
      "-fmax-errors=5"
)
