project(${PROJECT_NAME}-mlm
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "Fty common message bus sample linked with malamute library"
        LANGUAGES CXX)


etn_target(exe ${PROJECT_NAME}-example
  SOURCES
      fty_common_messagebus_example.cpp
  USES
      ${PROJECT_NAME_UNDERSCORE}
      fty-common-messagebus-sample-dto
      cxxtools
)

# etn_target(exe fty_common_messagebus_example2
#     SOURCES
#         fty_common_messagebus_example2.cc
#     USES
#         ${PROJECT_NAME_UNDERSCORE}
#         cxxtools
# )

# etn_target(exe fty_common_messagebus_example_req
#     SOURCES
#         fty_common_messagebus_example_req.cc
#     USES
#         ${PROJECT_NAME_UNDERSCORE}
#         cxxtools
# )

# etn_target(exe fty_common_messagebus_example_rep
#     SOURCES
#         fty_common_messagebus_example_rep.cc
#     USES
#         ${PROJECT_NAME_UNDERSCORE}
#         cxxtools
# )

# etn_target(exe fty_common_messagebus_example_rep2
#     SOURCES
#         fty_common_messagebus_example_rep2.cc
#     USES
#         ${PROJECT_NAME_UNDERSCORE}
#         cxxtools
# )
