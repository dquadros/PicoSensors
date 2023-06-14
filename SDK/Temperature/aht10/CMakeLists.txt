cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(aht10_project)

pico_sdk_init()

add_executable(aht10
    aht10_sdk.c
)


target_link_libraries(aht10 PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_i2c
)

pico_enable_stdio_usb(aht10 1)
pico_enable_stdio_uart(aht10 0)

pico_add_extra_outputs(aht10)
