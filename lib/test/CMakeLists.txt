# Test target
etn_test_target(${PROJECT_NAME_UNDERSCORE}
  SOURCES
    src/*.cpp
  USES
    ${PROJECT_NAME_UNDERSCORE}
    # ${SAMPLE_DTO_LIB_NAME}
)
