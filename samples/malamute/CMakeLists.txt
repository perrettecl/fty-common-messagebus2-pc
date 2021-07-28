project(${PROJECT_NAME}-mlm
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "fty messagebus sample for malamute"
        LANGUAGES CXX
)

# etn_target(exe ${PROJECT_NAME}-sample PRIVATE
#   SOURCES
#     src/*MlmSample.cpp
#   INCLUDE_DIRS
#     private_include
#   USES_PRIVATE
#     ${PROJECT_NAME_UNDERSCORE}
#     ${SAMPLE_DTO_LIB_NAME}

# )

# etn_target(exe ${PROJECT_NAME}-sample2 PRIVATE
#   SOURCES
#     src/*MlmSample2.cpp
#   INCLUDE_DIRS
#     private_include
#   USES_PRIVATE
#     ${PROJECT_NAME_UNDERSCORE}
#     ${SAMPLE_DTO_LIB_NAME}
# )

etn_target(exe ${PROJECT_NAME}-rep PRIVATE
  SOURCES
    src/*MlmSampleRep.cpp
  INCLUDE_DIRS
    private_include
  USES_PRIVATE
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
)

# etn_target(exe ${PROJECT_NAME}-rep2 PRIVATE
#   SOURCES
#     src/*MlmSampleRep2.cpp
#   INCLUDE_DIRS
#     private_include
#   USES_PRIVATE
#     ${PROJECT_NAME_UNDERSCORE}
#     ${SAMPLE_DTO_LIB_NAME}
# )

etn_target(exe ${PROJECT_NAME}-req PRIVATE
  SOURCES
    src/*MlmSampleReq.cpp
  INCLUDE_DIRS
    private_include
  USES_PRIVATE
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
)
