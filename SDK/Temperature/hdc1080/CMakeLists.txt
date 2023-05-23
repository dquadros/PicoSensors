cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(hdc1080_project)

pico_sdk_init()

add_executable(hdc1080
    hdc1080_sdk.c
)


target_link_libraries(hdc1080 PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_i2c
)

pico_enable_stdio_usb(hdc1080 1)
pico_enable_stdio_uart(hdc1080 0)

pico_add_extra_outputs(hdc1080)
