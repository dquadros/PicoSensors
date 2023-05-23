cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(potentiometer_project)

pico_sdk_init()

add_executable(potentiometer
    potentiometer_sdk.c
)


target_link_libraries(potentiometer PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_adc
    hardware_pwm
)

pico_enable_stdio_usb(potentiometer 1)
pico_enable_stdio_uart(potentiometer 0)

pico_add_extra_outputs(potentiometer)
