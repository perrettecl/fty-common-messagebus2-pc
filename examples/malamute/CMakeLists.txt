project(${PROJECT_NAME}-mlm
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "Fty common message bus sample linked with malamute library"
        LANGUAGES CXX
)

etn_target(exe ${PROJECT_NAME}-example
  SOURCES
    src/FtyCommonMessagebusExample.cpp
  USES_PRIVATE
    fty-messagebus-common
    fty-messagebus-utils
    fty-messagebus-mlm
    fty-messagebus-mqtt
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
    cxxtools
    czmq
    fty_common_mlm
    mlm
    pthread
    paho-mqttpp3
    jsoncpp
)

etn_target(exe ${PROJECT_NAME}-example2
  SOURCES
    src/FtyCommonMessagebusExample2.cpp
  USES_PRIVATE
    fty-messagebus-common
    fty-messagebus-utils
    fty-messagebus-mlm
    fty-messagebus-mqtt
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
    cxxtools
    czmq
    fty_common_mlm
    mlm
    pthread
    paho-mqttpp3
    jsoncpp
)

etn_target(exe ${PROJECT_NAME}-rep
  SOURCES
    src/FtyCommonMessagebusExampleRep.cpp
  USES_PRIVATE
    fty-messagebus-common
    fty-messagebus-utils
    fty-messagebus-mlm
    fty-messagebus-mqtt
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
    cxxtools
    czmq
    fty_common_mlm
    mlm
    pthread
    paho-mqttpp3
    jsoncpp
)

etn_target(exe ${PROJECT_NAME}-rep2
  SOURCES
    src/FtyCommonMessagebusExampleRep2.cpp
  USES_PRIVATE
    fty-messagebus-common
    fty-messagebus-utils
    fty-messagebus-mlm
    fty-messagebus-mqtt
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
    cxxtools
    czmq
    fty_common_mlm
    mlm
    pthread
    paho-mqttpp3
    jsoncpp
)

etn_target(exe ${PROJECT_NAME}-req
  SOURCES
    src/FtyCommonMessagebusExampleReq.cpp
  USES_PRIVATE
    fty-messagebus-common
    fty-messagebus-utils
    fty-messagebus-mlm
    fty-messagebus-mqtt
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
    cxxtools
    czmq
    fty_common_mlm
    mlm
    pthread
    paho-mqttpp3
    jsoncpp
)
