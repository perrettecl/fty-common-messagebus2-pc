cmake_minimum_required(VERSION 3.13)
cmake_policy(VERSION 3.13)

project(fty-common-messagebus-evol-project
    VERSION 1.0.0
    DESCRIPTION "fty common messagebus evolution"
)

option(BUILD_AMQP "Build AMQP implementation" OFF)
option(BUILD_MALAMUTE "Build Malamute implementation" ON)
option(BUILD_MQTT "Build MQTT implementation" ON)

option(BUILD_EXAMPLES "Build samples" OFF)
option(BUILD_TESTING "Build tests" OFF)
option(BUILD_DOC "Build documentation" OFF)

set(PROJECT_NAME_UNDERSCORE fty_common_messagebus_evol)

##############################################################################################################
find_package(fty-cmake PATHS ${CMAKE_BINARY_DIR}/fty-cmake REQUIRED)
##############################################################################################################

# Modules, interfaces, etc.
add_subdirectory(${PROJECT_SOURCE_DIR}/utils)
add_subdirectory(${PROJECT_SOURCE_DIR}/common)

# Implementation modules.
if(BUILD_MQTT)
  add_subdirectory(${PROJECT_SOURCE_DIR}/mqtt)
endif()
if(BUILD_AMQP)
  add_subdirectory(${PROJECT_SOURCE_DIR}/amqp)
endif()
if(BUILD_MALAMUTE)
  add_subdirectory(${PROJECT_SOURCE_DIR}/malamute)
endif()

# Library
add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
# Tools
add_subdirectory(${PROJECT_SOURCE_DIR}/tools)

# Examples
if(BUILD_EXAMPLES)
  add_subdirectory(examples)
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
