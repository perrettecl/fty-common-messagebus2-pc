
project(fty-messagebus-mlm
  VERSION ${PROJECT_VERSION}
  DESCRIPTION "Common messagebus mqtt implementation"
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
    fty_common_mlm
    czmq
  USES
    cxxtools
    # czmq
    # fty_common_mlm
    # mlm
    # pthread
  FLAGS
      "-fmax-errors=5"
)
