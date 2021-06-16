project(${SAMPLE_DTO_LIB_NAME}
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "fty common message bus sample dto shared with mlm and mqtt samples"
        LANGUAGES CXX)

##############################################################################################################
find_package(nlohmann_json 3.2.0 REQUIRED)
##############################################################################################################

etn_target(shared ${PROJECT_NAME}
  SOURCES
    src/*.cpp
  PUBLIC_INCLUDE_DIR
    public_include
  USES_PUBLIC
    fty_common_logging
    fty-messagebus-common
    fty-messagebus-mqtt
    nlohmann_json::nlohmann_json
)
