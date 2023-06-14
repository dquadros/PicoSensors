cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(bme280_project)

pico_sdk_init()

add_executable(bme280
    bme280_sdk.cpp
)


target_link_libraries(bme280 PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_i2c
)

pico_enable_stdio_usb(bme280 1)
pico_enable_stdio_uart(bme280 0)

pico_add_extra_outputs(bme280)
