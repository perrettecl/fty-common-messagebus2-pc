project(${PROJECT_NAME}-mlm
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "fty messagebus sample for malamute"
        LANGUAGES CXX
)

etn_target(exe ${PROJECT_NAME}-example PRIVATE
  SOURCES
    src/FtyCommonMessagebusExample.cpp
  INCLUDE_DIRS
    private_include
  USES_PRIVATE
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}

)

etn_target(exe ${PROJECT_NAME}-example2 PRIVATE
  SOURCES
    src/FtyCommonMessagebusExample2.cpp
  INCLUDE_DIRS
    private_include
  USES_PRIVATE
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
)

etn_target(exe ${PROJECT_NAME}-rep PRIVATE
  SOURCES
    src/FtyCommonMessagebusExampleRep.cpp
  INCLUDE_DIRS
    private_include
  USES_PRIVATE
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
)

etn_target(exe ${PROJECT_NAME}-rep2 PRIVATE
  SOURCES
    src/FtyCommonMessagebusExampleRep2.cpp
  INCLUDE_DIRS
    private_include
  USES_PRIVATE
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
)

etn_target(exe ${PROJECT_NAME}-req PRIVATE
  SOURCES
    src/FtyCommonMessagebusExampleReq.cpp
  INCLUDE_DIRS
    private_include
  USES_PRIVATE
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
)
