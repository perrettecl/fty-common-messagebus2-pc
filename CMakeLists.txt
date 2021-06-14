cmake_minimum_required(VERSION 3.13)
cmake_policy(VERSION 3.13)

project(fty-common-messagebus-evol-project
    VERSION 1.0.0
    DESCRIPTION "Common messagebus evol interface and implementation"
)

option(ENABLE_EXAMPLES "Build samples" TRUE)
set(PROJECT_NAME_UNDERSCORE fty_common_messagebus_evol)

##############################################################################################################
find_package(fty-cmake PATHS ${CMAKE_BINARY_DIR}/fty-cmake REQUIRED)
find_package(PahoMqttCpp REQUIRED)
find_package(jsoncpp REQUIRED)
##############################################################################################################

add_subdirectory(${PROJECT_SOURCE_DIR}/utils)
add_subdirectory(${PROJECT_SOURCE_DIR}/common)
add_subdirectory(${PROJECT_SOURCE_DIR}/mqtt)
add_subdirectory(${PROJECT_SOURCE_DIR}/malamute)

# Library
add_subdirectory(${PROJECT_SOURCE_DIR}/lib)

#### Examples
if(ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()

##############################################################################################################

# # Create a target for the tests
# etn_test_target(${PROJECT_NAME_UNDERSCORE}
#   SOURCES
#     test/main.cpp
#     test/dispatcher.cpp
#     test/pool_worker.cpp
#   INCLUDE_DIRS
#     private_include
#   USES
#     ${PROJECT_NAME_UNDERSCORE}
# )

##############################################################################################################
