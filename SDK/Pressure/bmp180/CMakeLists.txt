cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(bmp180_project)

pico_sdk_init()

add_executable(bmp180
    bmp180_sdk.cpp
)


target_link_libraries(bmp180 PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_i2c
)

pico_enable_stdio_usb(bmp180 1)
pico_enable_stdio_uart(bmp180 0)

pico_add_extra_outputs(bmp180)
