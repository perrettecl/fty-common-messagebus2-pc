cmake_minimum_required(VERSION 3.13)
cmake_policy(VERSION 3.13)

project(fty-common-messagebus-evol
    VERSION 1.0.0
    DESCRIPTION "Common messagebus interface"
)

set(PROJECT_NAME_UNDERSCORE fty_common_messagebus_evol)

option(ENABLE_EXAMPLES "Build samples" TRUE)

##############################################################################################################
find_package(fty-cmake PATHS ${CMAKE_BINARY_DIR}/fty-cmake REQUIRED)
find_package(PahoMqttCpp REQUIRED)
##############################################################################################################


##############################################################################################################
etn_target(shared ${PROJECT_NAME_UNDERSCORE}
  PUBLIC_INCLUDE_DIR
    public_include
  INCLUDE_DIRS
    private_include
  PUBLIC_HEADERS
    *.hpp
  SOURCES
    src/*.cpp
  USES_PUBLIC
    fty_common_logging
  USES
    cxxtools
    czmq
    fty_common_mlm
    mlm
    pthread
    paho-mqttpp3
    jsoncpp
  FLAGS
      "-fmax-errors=5"
)

etn_target(exe fty-msgbus-mlm-cli
  INCLUDE_DIRS
    private_include
  SOURCES
    src/MsgBusCli.cpp
  USES
    ${PROJECT_NAME_UNDERSCORE}
    cxxtools
)

set_target_properties(${PROJECT_NAME_UNDERSCORE} PROPERTIES SOVERSION ${PROJECT_VERSION_MAJOR})

##############################################################################################################

#### Examples
if(ENABLE_EXAMPLES)
  add_subdirectory(examples)
endif()

##############################################################################################################

# Create a target for the tests
etn_test_target(${PROJECT_NAME_UNDERSCORE}
  SOURCES
    test/main.cpp
    test/dispatcher.cpp
    test/pool_worker.cpp
  INCLUDE_DIRS
    private_include
  USES
    ${PROJECT_NAME_UNDERSCORE}
)

##############################################################################################################
