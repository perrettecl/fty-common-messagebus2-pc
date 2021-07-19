project(fty-messagebus-common
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "fty messagebus common interface"
)

##############################################################################################################
pkg_check_modules(UUID REQUIRED uuid)
##############################################################################################################

etn_target(interface ${PROJECT_NAME}
  PUBLIC_INCLUDE_DIR
    public_include
  PUBLIC_HEADERS
    *.hpp
  SOURCES
    src/*.cpp
)

target_link_libraries(${PROJECT_NAME} INTERFACE uuid)
