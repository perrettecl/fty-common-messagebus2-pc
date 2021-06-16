# Create a target for the tests
etn_test_target(${PROJECT_NAME_UNDERSCORE}_test
  SOURCES
    main.cpp
    dispatcher.cpp
    pool_worker.cpp
  USES
    fty-messagebus-utils
    ${PROJECT_NAME_UNDERSCORE}
)
