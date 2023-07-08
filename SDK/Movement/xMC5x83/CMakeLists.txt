cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(xMC5x83_project)

pico_sdk_init()

add_executable(xMC5x83
    xMC5x83_sdk.cpp
)


target_link_libraries(xMC5x83 PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_i2c
)

pico_enable_stdio_usb(xMC5x83 1)
pico_enable_stdio_uart(xMC5x83 0)

pico_add_extra_outputs(xMC5x83)
