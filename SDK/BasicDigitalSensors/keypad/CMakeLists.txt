cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(keypad_project)

pico_sdk_init()

add_executable(keypad
    keypad_sdk.c
)

target_link_libraries(keypad PRIVATE
    pico_stdlib
    hardware_gpio
)

pico_enable_stdio_usb(keypad 1)
pico_enable_stdio_uart(keypad 0)

pico_add_extra_outputs(keypad)
