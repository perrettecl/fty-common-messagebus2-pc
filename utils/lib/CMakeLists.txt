project(fty-messagebus-utils
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "fty messagebus utilitary static library"
)

etn_target(static ${PROJECT_NAME} PUBLIC
  PUBLIC_INCLUDE_DIR
    public_include
  PUBLIC_HEADERS
    *.hpp
  SOURCES
    src/*.cpp
  USES_PUBLIC
    Threads::Threads
)
