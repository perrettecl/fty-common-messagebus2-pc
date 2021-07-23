
project(fty-messagebus-malamute
  VERSION ${PROJECT_VERSION}
  DESCRIPTION "fty messagebus malamute library"
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
  USES_PRIVATE
    fty-messagebus-utils
  USES_PUBLIC
    fty-messagebus-common
    fty_common_logging
    fty_common_mlm
    czmq
    mlm
)
