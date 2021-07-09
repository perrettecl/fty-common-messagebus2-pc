project(fty-messagebus-common-project
  VERSION ${PROJECT_VERSION}
  DESCRIPTION "fty messagebus common project"
)

add_subdirectory(${PROJECT_SOURCE_DIR}/lib)
# add_subdirectory(test)

# if(BUILD_TESTING)
#   etn_test_target(${PROJECT_NAME_UNDERSCORE}
#     SOURCES
#       test/src/*.cpp
#     USES
#       fty-messagebus-common
#   )
# endif()
