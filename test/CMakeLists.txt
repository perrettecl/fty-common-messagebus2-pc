# Create a target for the tests
etn_test_target(${PROJECT_NAME_UNDERSCORE}
  SOURCES
    src/*.cpp
  USES
    ${PROJECT_NAME_UNDERSCORE}
)
