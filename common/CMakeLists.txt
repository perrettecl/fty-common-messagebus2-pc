project(fty-common-messagebus2
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "fty messagebus 2 common interface"
)

##############################################################################################################
find_package(fty-cmake PATHS ${CMAKE_BINARY_DIR}/fty-cmake REQUIRED)
##############################################################################################################

etn_target(shared fty-common-messagebus2
  SOURCES
    src/*.cpp
  PUBLIC_INCLUDE_DIR
    public_include
  PUBLIC_HEADERS
    *.h
  USES_PRIVATE
    uuid
)

set_target_properties(${PROJECT_NAME} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})