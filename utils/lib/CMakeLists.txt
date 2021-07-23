project(fty-messagebus-utils
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "fty messagebus static library"
)

etn_target(static ${PROJECT_NAME} PRIVATE
  PUBLIC_INCLUDE_DIR
    public_include
  SOURCES
    src/*.cpp
  USES_PUBLIC
    Threads::Threads
)
