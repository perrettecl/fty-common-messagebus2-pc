
project(fty-messagebus-mqtt
  VERSION ${PROJECT_VERSION}
  DESCRIPTION "Common messagebus mqtt implementation"
)

##############################################################################################################
find_package(PahoMqttCpp REQUIRED)
find_package(jsoncpp REQUIRED)
# find_package(PkgConfig REQUIRED)
# pkg_check_modules(JSONCPP jsoncpp)
# message("FOUND CONFIG: ${jsoncpp_CONFIG}")
# message("INCLUDE DIRS: ${jsoncpp_INCLUDE_DIRS}")
message("LIBRARIES: ${jsoncpp_prefix}")
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
  # USES_PRIVATE
    # fty-messagebus-utils
    # fty-messagebus-common
  USES_PUBLIC
    fty-messagebus-utils
    fty-messagebus-common
    fty_common_logging
    pthread
    paho-mqttpp3
    paho-mqtt3as
    paho-mqtt3c
    jsoncpp
  USES
     jsoncpp
  FLAGS
      "-fmax-errors=5"
)
