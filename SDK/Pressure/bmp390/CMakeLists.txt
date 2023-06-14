cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(bmp390_project)
add_compile_options(-DBMP3_FLOAT_COMPENSATION)

pico_sdk_init()

add_executable(bmp390
    bmp390_sdk.c
    bmp3.c
)

target_link_libraries(bmp390 PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_i2c
)

pico_enable_stdio_usb(bmp390 1)
pico_enable_stdio_uart(bmp390 0)

pico_add_extra_outputs(bmp390)
