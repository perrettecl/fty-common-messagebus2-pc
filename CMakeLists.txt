cmake_minimum_required(VERSION 3.13)
cmake_policy(VERSION 3.13)

project(fty-common-messagebus-evol-project
    VERSION 1.0.0
    DESCRIPTION "fty common messagebus evolution"
)

macro(relative_option parent_option new_option_name text)
  if (${parent_option})
    option(${new_option_name} ${text} ON)
  else()
    option(${new_option_name} ${text} OFF)
  endif()
endmacro()

option(                            BUILD_ALL                   "Build all addons"       ON)
relative_option(${BUILD_ALL}       BUILD_AMQP                  "Build AMQP addon"         )
relative_option(${BUILD_ALL}       BUILD_MALAMUTE              "Build Malamute addon"     )
relative_option(${BUILD_ALL}       BUILD_MQTT                  "Build MQTT addon"         )

option(                            BUILD_SAMPLES               "Build samples"         OFF)
option(                            BUILD_TESTING               "Build tests"           OFF)
option(                            BUILD_DOC                   "Build documentation"   OFF)

set(PROJECT_NAME_UNDERSCORE fty_common_messagebus_evo)

##############################################################################################################
find_package(fty-cmake PATHS ${CMAKE_BINARY_DIR}/fty-cmake REQUIRED)
##############################################################################################################

# Modules, interfaces, etc.
add_subdirectory(${PROJECT_SOURCE_DIR}/utils)
add_subdirectory(${PROJECT_SOURCE_DIR}/common)
add_subdirectory(${PROJECT_SOURCE_DIR}/addons)

# Library
add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
# Tools
add_subdirectory(${PROJECT_SOURCE_DIR}/tools)

# Examples
if(BUILD_SAMPLES)
  add_subdirectory(samples)
endif()

##############################################################################################################

# # Create a target for the tests
etn_test_target(${PROJECT_NAME_UNDERSCORE}
  SOURCES
    # common/test/src/*.cpp
    test/main.cpp
    test/dispatcher.cpp
    test/pool_worker.cpp
  USES
    fty-messagebus-common
    fty-messagebus-utils
    ${PROJECT_NAME_UNDERSCORE}
)

##############################################################################################################

set_target_properties(${PROJECT_NAME_UNDERSCORE} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})
