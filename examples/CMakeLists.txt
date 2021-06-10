project(fty-common-messagebus-sample
        VERSION ${PROJECT_VERSION}
        DESCRIPTION "Fty common message bus sample"
        LANGUAGES CXX)


# Dto(s)
add_subdirectory(dto)
# Malamute samples
add_subdirectory(malamute)
# Mqtt samples
add_subdirectory(mqtt)
