cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(analoghall_project)

pico_sdk_init()

add_executable(analoghall
    analoghall_sdk.c
)


target_link_libraries(analoghall PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_adc
)

pico_enable_stdio_usb(analoghall 1)
pico_enable_stdio_uart(analoghall 0)

pico_add_extra_outputs(analoghall)
