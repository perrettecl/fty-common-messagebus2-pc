

if(BUILD_TESTING)
  enable_testing()

  etn_test(${PROJECT_NAME}-test
    SOURCES
        src/*.cpp
    USES
      ${PROJECT_NAME}
      Catch2::Catch2
  )
endif()
