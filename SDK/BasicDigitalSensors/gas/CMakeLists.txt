cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(gas_project)

pico_sdk_init()

add_executable(gas
    gas_sdk.c
)

target_link_libraries(gas PRIVATE
    pico_stdlib
    hardware_gpio
)

pico_add_extra_outputs(gas)
