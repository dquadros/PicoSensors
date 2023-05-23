cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(mcp9808_project)

pico_sdk_init()

add_executable(mcp9808
    mcp9808_sdk.c
)


target_link_libraries(mcp9808 PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_i2c
)

pico_enable_stdio_usb(mcp9808 1)
pico_enable_stdio_uart(mcp9808 0)

pico_add_extra_outputs(mcp9808)
