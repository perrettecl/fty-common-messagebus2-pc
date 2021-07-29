
project(fty_common_messagebus2_malamute
  VERSION ${PROJECT_VERSION}
  DESCRIPTION "fty messagebus2 malamute library"
)

##############################################################################################################

##############################################################################################################

etn_target(shared ${PROJECT_NAME} PUBLIC
  INCLUDE_DIRS
    private_include
  PUBLIC_INCLUDE_DIR
    public_include
  PUBLIC_HEADERS
    public_include
  SOURCES
    src/*.cpp
  USES_PUBLIC
    fty_common_messagebus_utils
    fty-messagebus-common
    fty_common_logging
    fty_common_mlm
    czmq
    mlm
)

set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
