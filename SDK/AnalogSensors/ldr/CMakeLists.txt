cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(ldr_project)

pico_sdk_init()

add_executable(ldr
    ldr_sdk.c
)


target_link_libraries(ldr PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_adc
)

pico_add_extra_outputs(ldr)
