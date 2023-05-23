cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(phototrans_project)

pico_sdk_init()

add_executable(phototrans
    phototrans_sdk.c
)


target_link_libraries(phototrans PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_adc
)

pico_enable_stdio_usb(phototrans 1)
pico_enable_stdio_uart(phototrans 0)

pico_add_extra_outputs(phototrans)
