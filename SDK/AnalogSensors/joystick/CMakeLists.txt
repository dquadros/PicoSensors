cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(joystick_project)

pico_sdk_init()

add_executable(joystick
    joy_sdk.cpp
)


target_link_libraries(joystick PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_adc
    hardware_pwm
)

pico_add_extra_outputs(joystick)
