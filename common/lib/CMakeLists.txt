project(fty-messagebus-common
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "Common interface for messagebus"
)

etn_target(interface ${PROJECT_NAME}
  PUBLIC_INCLUDE_DIR
    include
  PUBLIC_HEADERS
    *.hpp
)
