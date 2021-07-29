# Test target
etn_test_target(${PROJECT_NAME_UNDERSCORE}
  SOURCES
    # src/*.cpp
    src/MsgBus.cpp
  USES
    ${PROJECT_NAME_UNDERSCORE}
)
