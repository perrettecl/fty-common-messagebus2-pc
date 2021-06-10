project(${PROJECT_NAME}-mlm
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "Fty common message bus sample linked with malamute library"
        LANGUAGES CXX
)

etn_target(exe ${PROJECT_NAME}-example
  SOURCES
    src/FtyCommonMessagebusExample.cpp
  USES
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
    cxxtools
)

etn_target(exe ${PROJECT_NAME}-example2
  SOURCES
    src/FtyCommonMessagebusExample2.cpp
  USES
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
    cxxtools
)

etn_target(exe ${PROJECT_NAME}-req
  SOURCES
    src/FtyCommonMessagebusExampleReq.cpp
  USES
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
    cxxtools
)

etn_target(exe ${PROJECT_NAME}-rep
  SOURCES
    src/FtyCommonMessagebusExampleRep.cpp
  USES
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
    cxxtools
)

etn_target(exe ${PROJECT_NAME}-rep2
  SOURCES
    src/FtyCommonMessagebusExampleRep2.cpp
  USES
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
    cxxtools
)
