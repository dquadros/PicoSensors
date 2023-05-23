cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(ledsensor_project)

pico_sdk_init()

add_executable(ledsensor
    ledsensor_sdk.c
)


target_link_libraries(ledsensor PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_adc
)

pico_enable_stdio_usb(ledsensor 1)
pico_enable_stdio_uart(ledsensor 0)

pico_add_extra_outputs(ledsensor)
