cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(lm75a_project)

pico_sdk_init()

add_executable(lm75a
    lm75a_sdk.c
)


target_link_libraries(lm75a PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_i2c
)

pico_enable_stdio_usb(lm75a 1)
pico_enable_stdio_uart(lm75a 0)

pico_add_extra_outputs(lm75a)
