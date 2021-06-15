project(fty-messagebus-common
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "Common messagebus interface"
)

etn_target(interface ${PROJECT_NAME}
  PUBLIC_INCLUDE_DIR
    public_include
  PUBLIC_HEADERS
    *.hpp
)
