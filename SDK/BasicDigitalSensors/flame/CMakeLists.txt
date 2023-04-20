cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(flame_project)

pico_sdk_init()

add_executable(flame
    flame_sdk.c
)


target_link_libraries(flame PRIVATE
    pico_stdlib
    hardware_gpio
)

pico_add_extra_outputs(flame)
