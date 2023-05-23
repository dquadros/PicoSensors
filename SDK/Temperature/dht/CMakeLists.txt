cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(dht_project)

pico_sdk_init()

add_executable(dht
    dht_sdk.cpp
)

pico_generate_pio_header(dht ${CMAKE_CURRENT_LIST_DIR}/dht_sdk.pio)


target_link_libraries(dht PRIVATE
    pico_stdlib
    pico_stdio
    hardware_pio
)

pico_enable_stdio_usb(dht 1)
pico_enable_stdio_uart(dht 0)

pico_add_extra_outputs(dht)

