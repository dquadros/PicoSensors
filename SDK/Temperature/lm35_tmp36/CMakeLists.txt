cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(lm35_tmp36_project)

pico_sdk_init()

add_executable(lm35_tmp36
    lm35_tmp36_sdk.c
)


target_link_libraries(lm35_tmp36 PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_adc
)

pico_enable_stdio_usb(lm35_tmp36 1)
pico_enable_stdio_uart(lm35_tmp36 0)

pico_add_extra_outputs(lm35_tmp36)
