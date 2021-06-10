project(fty-common-messagebus-sample-dto
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "Fty common message bus sample dto shared with mlm, mqtt samples"
        LANGUAGES CXX)

etn_target(shared ${PROJECT_NAME}
  SOURCES
      src/*.cpp
  PUBLIC_INCLUDE_DIR
      public_include
)
