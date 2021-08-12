
project(fty-common-messagebus2-malamute
  VERSION 1.0.0
  DESCRIPTION "fty messagebus2 malamute library"
)

##############################################################################################################
find_package(fty-cmake PATHS ${CMAKE_BINARY_DIR}/fty-cmake REQUIRED)
##############################################################################################################

find_package(Threads REQUIRED)

etn_target(shared ${PROJECT_NAME} PUBLIC
  SOURCES
    src/*.cpp
    src/*.hpp
    src/*.h
  PUBLIC_INCLUDE_DIR
    public_include
  PUBLIC_HEADERS
    public_include
  USES
    fty_common_logging
    fty_common_mlm
    czmq
    mlm
  USES_PUBLIC
    fty-common-messagebus2
)

set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
