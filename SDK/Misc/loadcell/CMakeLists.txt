cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(loadcell_project)

pico_sdk_init()

add_executable(loadcell
    loadcell_sdk.c
    lcd_i2c.c
)

pico_generate_pio_header(loadcell ${CMAKE_CURRENT_LIST_DIR}/hx711.pio)

target_link_libraries(loadcell PRIVATE
    pico_stdlib
    pico_stdio
    hardware_pio
    hardware_i2c
)

pico_enable_stdio_usb(loadcell 0)
pico_enable_stdio_uart(loadcell 0)

pico_add_extra_outputs(loadcell)

