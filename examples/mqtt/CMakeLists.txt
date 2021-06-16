project(${PROJECT_NAME}-mqtt
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "fty messagebus sample for mqtt"
        LANGUAGES CXX
)

set (THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

# Pub/Sub
etn_target(exe ${PROJECT_NAME}-pub-sub
  SOURCES
    src/*PubSub.cpp
  INCLUDE_DIRS
    private_include
  USES_PRIVATE
    fty-messagebus-common
    fty-messagebus-utils
    fty-messagebus-mlm
    fty-messagebus-mqtt
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
  FLAGS
    "-fmax-errors=1"
)

# Math sample replyer
etn_target(exe ${PROJECT_NAME}-async-reply
  SOURCES
    src/*SampleAsyncReply.cpp
  INCLUDE_DIRS
    private_include
  USES_PUBLIC
    fty-messagebus-common
    fty-messagebus-utils
    fty-messagebus-mlm
    fty-messagebus-mqtt
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
  FLAGS
    "-fmax-errors=1"
)

# Math sample asynchrone and synchrone requester
etn_target(exe ${PROJECT_NAME}-send-request
  SOURCES
    src/*SampleSendRequest.cpp
  INCLUDE_DIRS
     private_include
  USES_PUBLIC
    fty-messagebus-common
    fty-messagebus-utils
    fty-messagebus-mlm
    fty-messagebus-mqtt
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
  FLAGS
    "-fmax-errors=1"
)

# Stress lib sample
etn_target(exe ${PROJECT_NAME}-stress-lib
  SOURCES
    src/*SampleStressLib.cpp
  INCLUDE_DIRS
    private_include
  USES_PUBLIC
    fty-messagebus-common
    fty-messagebus-utils
    fty-messagebus-mlm
    fty-messagebus-mqtt
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
  FLAGS
    "-fmax-errors=1"
)

# Req/Rep multi threaded
etn_target(exe ${PROJECT_NAME}-multi-thread-req-rep
  SOURCES
    src/*SampleMultiThreadReqRep.cpp
  INCLUDE_DIRS
    private_include
  USES_PUBLIC
    fty-messagebus-common
    fty-messagebus-utils
    fty-messagebus-mlm
    fty-messagebus-mqtt
    ${PROJECT_NAME_UNDERSCORE}
    ${SAMPLE_DTO_LIB_NAME}
    Threads::Threads
  FLAGS
    "-fmax-errors=1"
)
