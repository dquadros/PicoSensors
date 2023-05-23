/**
 * @file analoghall_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Analog Hall Effect Sensor Example
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

// Reading for no magnetic field
int zero;

// Read sensor (average 50 readings)
#define N_READINGS 50
uint readSensor() {
  uint32_t sum = 0;
  for (int i = 0; i < N_READINGS; i++) {
    sum += adc_read();
  }
  return sum / N_READINGS;
}


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

  // Assume no magnetic field at the start
  zero = readSensor() ;
  printf("Zero = %d\n", zero);
  sleep_ms(1000);
  printf ("Ready\n");

  // Main loop
  while(1) {
    int field = readSensor();
    if (((field-zero) < 30) && ((field-zero) > -30)) {
      printf ("No field\n");
    } else if (field > zero) {
      printf ("Field = S %d\n", field-zero);
    } else {
      printf ("Field = N %d\n", zero-field);
    }
    sleep_ms(1000);
  }
}
