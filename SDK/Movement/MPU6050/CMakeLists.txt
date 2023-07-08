cmake_minimum_required(VERSION 3.13)

include(pico_sdk_import.cmake)

project(mpu6050_project)

pico_sdk_init()

add_executable(mpu6050
    mpu6050_sdk.cpp
)


target_link_libraries(mpu6050 PRIVATE
    pico_stdlib
    hardware_gpio
    hardware_i2c
)

pico_enable_stdio_usb(mpu6050 1)
pico_enable_stdio_uart(mpu6050 0)

pico_add_extra_outputs(mpu6050)
