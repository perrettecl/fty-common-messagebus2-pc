
project(fty-messagebus-mlm
  VERSION ${PROJECT_VERSION}
  DESCRIPTION "Common messagebus malamute implementation"
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
