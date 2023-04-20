cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(hall_project)

pico_sdk_init()

add_executable(hall
    hall_sdk.cpp
)

target_link_libraries(hall PRIVATE
    pico_stdlib
    hardware_gpio
)

pico_enable_stdio_usb(hall 1)
pico_enable_stdio_uart(hall 0)



pico_add_extra_outputs(hall)
