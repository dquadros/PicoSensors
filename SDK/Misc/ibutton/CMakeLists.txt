cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(ibutton_project)

pico_sdk_init()

add_subdirectory(pico-onewire)

add_executable(ibutton
    ibutton_sdk.cpp
)


target_link_libraries(ibutton PRIVATE
    pico_stdlib
    hardware_gpio
    pico_one_wire
)

pico_enable_stdio_usb(ibutton 1)
pico_enable_stdio_uart(ibutton 0)

pico_add_extra_outputs(ibutton)
