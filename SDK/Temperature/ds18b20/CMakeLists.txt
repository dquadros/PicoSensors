cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(ds18b20_project)

pico_sdk_init()

add_subdirectory(pico-onewire)

add_executable(ds18b20
    ds18b20_sdk.cpp
)


target_link_libraries(ds18b20 PRIVATE
    pico_stdlib
    hardware_gpio
    pico_one_wire
)

pico_enable_stdio_usb(ds18b20 1)
pico_enable_stdio_uart(ds18b20 0)

pico_add_extra_outputs(ds18b20)
