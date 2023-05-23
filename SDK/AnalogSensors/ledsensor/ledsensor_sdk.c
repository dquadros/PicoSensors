/**
 * @file ledsensor_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Phototransistor Example
 * @version 1.0
 * @date 2023-05-18
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

// Pins
#define SENSOR_PIN 26

// Main Program
int main() {
    // Init stdio
    stdio_init_all();
    #ifdef LIB_PICO_STDIO_USB
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    #endif

    // Init sensor ADC
    adc_init();
    adc_gpio_init(SENSOR_PIN);
    adc_select_input(0);

    // Main loop
    while(1) {
      uint32_t sum = 0;
      for (int i = 0; i < 20; i++) {
        sum +=adc_read();
      }
      printf("%u\n", sum/20);
      sleep_ms(2000);
    }
}
