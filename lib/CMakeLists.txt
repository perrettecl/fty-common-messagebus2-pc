project(fty-common-messagebus-evol
    VERSION 1.0.0
    DESCRIPTION "fty common messagebus library evolution"
)

##############################################################################################################
etn_target(shared ${PROJECT_NAME_UNDERSCORE}
  PUBLIC_INCLUDE_DIR
    public_include
  PUBLIC_HEADERS
    *.hpp
  SOURCES
    src/*.cpp
  USES_PUBLIC
    fty-messagebus-common
    fty-messagebus-utils
    fty-messagebus-mlm
    fty-messagebus-mqtt
    fty_common_logging
  FLAGS
      "-fmax-errors=5"
)
