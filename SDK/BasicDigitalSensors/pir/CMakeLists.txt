cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(pir_project)

pico_sdk_init()

add_executable(pir
    pir_sdk.c
)

target_link_libraries(pir PRIVATE
    pico_stdlib
    hardware_gpio
)

pico_enable_stdio_usb(pir 1)
pico_enable_stdio_uart(pir 0)

pico_add_extra_outputs(pir)
