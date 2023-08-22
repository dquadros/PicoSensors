cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(fingerprint_project)

pico_sdk_init()

add_executable(fingerprint
    fingerprint_sdk.cpp fpm10a_sdk.cpp
)


target_link_libraries(fingerprint PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_uart
)

pico_enable_stdio_usb(fingerprint 1)
pico_enable_stdio_uart(fingerprint 0)

pico_add_extra_outputs(fingerprint)
