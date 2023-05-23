cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(thermistor_project)

pico_sdk_init()

add_executable(thermistor
    thermistor_sdk.c
)


target_link_libraries(thermistor PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_adc
)

pico_enable_stdio_usb(thermistor 1)
pico_enable_stdio_uart(thermistor 0)

pico_add_extra_outputs(thermistor)
