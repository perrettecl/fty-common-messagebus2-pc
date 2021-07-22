project(${PROJECT_NAME}-sample
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "fty messagebus sample"
        LANGUAGES CXX)


set (SAMPLE_DTO_LIB_NAME fty-common-messagebus-evol-sample-dto)
# Dto(s)
add_subdirectory(dto)
# Malamute samples
add_subdirectory(malamute)
# Mqtt samples
add_subdirectory(mqtt)
