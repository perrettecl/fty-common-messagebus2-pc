# fty-common-messagebus-evol

## Dependencies
* [fty-cmake](https://github.com/42ity/fty-cmake/)
* [fty_common_mlm](https://github.com/42ity/fty-common-mlm)
* [fty_common_logging](https://github.com/42ity/fty-common-logging)
* [PahoMqttC](https://github.com/eclipse/paho.mqtt.c)
* [PahoMqttCpp](https://github.com/eclipse/paho.mqtt.cpp)
* [googletest](https://github.com/google/googletest)

## Description
This project is developped on the purpose to provide somme common methods to address Request/Reply, and Publish/Subscribe patterns above malamute or Mqtt message bus.

## How to build

To build fty-common-messagebus-evol project run:

```cmake
cmake -B build -DENABLE_EXAMPLES=ON|OFF -DBUILD_TESTING=ON|OFF
cmake --build build
```

## How to use the dependency in your project

Add the dependency in CMakeList.txt:

```cmake
etn_target(${PROJECT_NAME}
  SOURCES
    .....
  USES_PUBLIC
    .....
    fty_common_messagebus_evol
    .....
)
```

## Howto

See all samples in examples folder.

## Change log

[Change log](CHANGELOG.md) provides informations about bug fixing, improvement, etc.
