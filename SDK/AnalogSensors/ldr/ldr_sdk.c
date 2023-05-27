/**
 * @file ldr_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief LDR Example
 * @version 1.0
 * @date 2023-05-16
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"

// Pins
#define BUZZER_PIN 15
#define SENSOR_PIN 26

// Ligh/Dark threshold
#define DARK 300


// Main Program
int main() {
    // Init Buzzer gpio
    gpio_init (BUZZER_PIN);
    gpio_set_dir (BUZZER_PIN, true);
    gpio_put (BUZZER_PIN, false);

    // Init sensor ADC
    adc_init();
    adc_gpio_init(SENSOR_PIN);
    adc_select_input(0);

    // Main loop
    while(1) {
      // Sleep for 3 to 60 seconds
      uint delay = (rand() % 57000) + 3000;
      sleep_ms (delay);
      // Short beep if dark
      uint16_t val = adc_read(); // 0-4095
      if (val < DARK) {
        gpio_put (BUZZER_PIN, true);
        sleep_ms (100);
        gpio_put (BUZZER_PIN, false);
      }
    }
}
