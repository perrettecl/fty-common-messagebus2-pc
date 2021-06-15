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
  FLAGS
      "-fmax-errors=5"
)
