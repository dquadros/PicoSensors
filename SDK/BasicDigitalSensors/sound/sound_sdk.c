/**
 * @file sound_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Sound Sensor Example
 * @version 1.0
 * @date 2023-04-19
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"


#define LED_PIN 25
#define SENSOR_PIN 16

// Main Program
int main() {
    // Init LED gpio
    gpio_init (LED_PIN);
    gpio_set_dir (LED_PIN, true);
    gpio_put (LED_PIN, false);

    // Init sensor gpio
    gpio_init (SENSOR_PIN);

    // Main loop
    while(1) {
      if (gpio_get(SENSOR_PIN) == 1) {
        gpio_put (LED_PIN, true);
        sleep_ms (500);
        gpio_put (LED_PIN, false);
      }
    }
}