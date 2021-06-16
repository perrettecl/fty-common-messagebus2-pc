project(fty-messagebus-common
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "fty messagebus common interface"
)

etn_target(interface ${PROJECT_NAME}
  PUBLIC_INCLUDE_DIR
    public_include
  PUBLIC_HEADERS
    *.hpp
)
