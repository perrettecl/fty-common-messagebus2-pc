
project(fty-messagebus-mlm
  VERSION ${PROJECT_VERSION}
  DESCRIPTION "fty messagebus malamute library"
)

##############################################################################################################

##############################################################################################################


##############################################################################################################
etn_target(static ${PROJECT_NAME}
  PUBLIC_INCLUDE_DIR
    public_include
  PUBLIC_HEADERS
    *.hpp
  SOURCES
    src/*.cpp
  USES_PRIVATE
    fty-messagebus-utils
    fty-messagebus-common
  USES_PUBLIC
    fty_common_logging
    mlm
  USES
    cxxtools
    czmq
    fty_common_mlm
    mlm
  FLAGS
    "-fmax-errors=1"
)
