project(${PROJECT_NAME}-mlm
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "fty messagebus sample for malamute"
        LANGUAGES CXX
)

etn_target(exe ${PROJECT_NAME}-request PRIVATE
  SOURCES
    src/*MlmSampleRequest.cpp
  INCLUDE_DIRS
    private_include
  USES_PRIVATE
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
)

etn_target(exe ${PROJECT_NAME}-reply PRIVATE
  SOURCES
    src/*MlmSampleReply.cpp
  INCLUDE_DIRS
    private_include
  USES_PRIVATE
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
)

etn_target(exe ${PROJECT_NAME}-RequestReply PRIVATE
  SOURCES
    src/*MlmSampleRequestReply.cpp
  INCLUDE_DIRS
    private_include
  USES_PRIVATE
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
)
