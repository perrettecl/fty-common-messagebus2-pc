etn_target(exe fty-msgbus-mlm-cli PUBLIC
  SOURCES
    src/*.cpp
  USES_PRIVATE
    fty-messagebus-malamute
    ${PROJECT_NAME_UNDERSCORE}
)
