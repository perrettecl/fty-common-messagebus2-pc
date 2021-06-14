project(${SAMPLE_DTO_LIB_NAME}
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "Fty common message bus sample dto shared with mlm, mqtt samples"
        LANGUAGES CXX)

etn_target(interface ${PROJECT_NAME}
  SOURCES
      src/*.cpp
  PUBLIC_INCLUDE_DIR
      public_include
  # USES_PRIVATE
      # fty-messagebus-common
      # fty-messagebus-utils
      # fty-messagebus-mlm
      # fty-messagebus-mqtt
      # jsoncpp
  USES_PUBLIC
      fty_common_logging
      fty-messagebus-common
      jsoncpp
)
