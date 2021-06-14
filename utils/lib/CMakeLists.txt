project(fty-messagebus-utils
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "Common interface for messagebus"
)

etn_target(interface ${PROJECT_NAME}
  PUBLIC
    *.hpp
  PUBLIC_INCLUDE_DIR
    include
  SOURCES
    src/*.cpp
  USES_PUBLIC
    czmq
)
