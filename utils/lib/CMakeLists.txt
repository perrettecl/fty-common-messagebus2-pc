project(fty-messagebus-utils
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "fty messagebus utils interface"
)

etn_target(static ${PROJECT_NAME}
  PUBLIC
    *.hpp
  PUBLIC_INCLUDE_DIR
    public_include
  SOURCES
    src/*.cpp
  USES_PUBLIC
    czmq
)
