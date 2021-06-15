project(fty-messagebus-utils
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "Common messagebus utils library"
)

etn_target(interface ${PROJECT_NAME}
  PUBLIC
    *.hpp
  PUBLIC_INCLUDE_DIR
    public_include
  SOURCES
    src/*.cpp
  USES_PUBLIC
    czmq
)
