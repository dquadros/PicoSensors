cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(button_project)

pico_sdk_init()

add_executable(button
    button_sdk.cpp
)


target_link_libraries(button PRIVATE
    pico_stdlib
    hardware_gpio
)

pico_enable_stdio_usb(button 1)
pico_enable_stdio_uart(button 0)

pico_add_extra_outputs(button)
