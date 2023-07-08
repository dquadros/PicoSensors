cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(mma8452_project)

pico_sdk_init()

add_executable(mma8452
    mma8452_sdk.cpp
)


target_link_libraries(mma8452 PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_i2c
)

pico_enable_stdio_usb(mma8452 1)
pico_enable_stdio_uart(mma8452 0)

pico_add_extra_outputs(mma8452)
