cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(rfid125_project)

pico_sdk_init()

add_executable(rfid125
    rfid125_sdk.cpp
)


target_link_libraries(rfid125 PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_uart
    hardware_pwm
)

pico_enable_stdio_usb(rfid125 1)
pico_enable_stdio_uart(rfid125 0)

pico_add_extra_outputs(rfid125)
