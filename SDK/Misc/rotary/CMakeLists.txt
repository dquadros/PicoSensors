cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(rotary_project)

pico_sdk_init()

add_executable(rotary
    rotary_sdk.cpp
)

pico_generate_pio_header(rotary ${CMAKE_CURRENT_LIST_DIR}/encoder.pio)


target_link_libraries(rotary PRIVATE
    pico_stdlib
    pico_stdio
    hardware_pio
)

pico_enable_stdio_usb(rotary 1)
pico_enable_stdio_uart(rotary 0)

pico_add_extra_outputs(rotary)

