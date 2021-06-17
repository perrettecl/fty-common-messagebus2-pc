
project(fty-messagebus-mqtt
  VERSION ${PROJECT_VERSION}
  DESCRIPTION "fty messagebus mqtt library"
)

##############################################################################################################
find_package(PahoMqttCpp REQUIRED)
##############################################################################################################

etn_target(static ${PROJECT_NAME} PRIVATE
  INCLUDE_DIRS
    private_include
  PUBLIC_INCLUDE_DIR
    public_include
  PUBLIC_HEADERS
    public_include
  SOURCES
    src/*.cpp
  USES_PRIVATE
    fty-messagebus-utils
  USES_PUBLIC
    fty-messagebus-common
    fty_common_logging
    pthread
  FLAGS
    "-fmax-errors=1"
)
