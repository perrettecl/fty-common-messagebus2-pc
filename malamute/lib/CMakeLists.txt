
project(fty-messagebus-mlm
  VERSION ${PROJECT_VERSION}
  DESCRIPTION "fty messagebus malamute library"
)

##############################################################################################################

##############################################################################################################


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
    fty_common_mlm
  FLAGS
    "-fmax-errors=1"
)
