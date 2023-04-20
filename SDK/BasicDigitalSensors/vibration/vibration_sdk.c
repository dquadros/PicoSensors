/**
 * @file vibration_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Vibration Sensor Example
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

// Previous readings
#define N_SAMPLES 100
uint8_t vibr[N_SAMPLES];
int inVibr, outVibr, nVibr;
int count;

// Main Program
int main() {
    // Init LED gpio
    gpio_init (LED_PIN);
    gpio_set_dir (LED_PIN, true);
    gpio_put (LED_PIN, false);

    // Init sensor gpio
    gpio_init (SENSOR_PIN);

    // Init samples
    inVibr = outVibr = nVibr = 0;
    count = 0;

    // Main loop
    while(1) {
      sleep_ms(10);
      if (nVibr == N_SAMPLES) {
        // Remove oldest
        count -= vibr[outVibr];
        outVibr = (outVibr+1) % N_SAMPLES;
      } else {
        nVibr++;
      }
      vibr[inVibr] = gpio_get(SENSOR_PIN);
      count += vibr[inVibr];
      inVibr = (inVibr+1) % N_SAMPLES;
      gpio_put (LED_PIN, count > 80);
    }
}