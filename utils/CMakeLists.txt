project(fty-messagebus-utils-project
  VERSION ${PROJECT_VERSION}
  DESCRIPTION "fty messagebus utils project"
)

add_subdirectory(lib)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()
