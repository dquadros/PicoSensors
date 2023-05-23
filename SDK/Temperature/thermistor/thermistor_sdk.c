/**
 * @file thermistor_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Thermistor Example
 * @version 1.0
 * @date 2023-05-19
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"

// Thermistor connection
#define THERMISTOR_PIN 26

// Circuit Parameters
const float Vcc = 3.3;  
const float R = 2200.0; 

// Parameters
typedef struct {
  float r;
  float t;
} ResTemp;

// Returns the thermistor resistance
// uses a mean of 20 ADC readings
float getResistance() {
  uint32_t sum = 0;
  for (int i = 0; i < 20; i++) {
    sum += adc_read();
  }
  float v = (Vcc*sum)/(20*4096.0);
  return R*(Vcc-v)/v;
}

// Gets a reference temperature
void getReference(ResTemp *ref) {
  printf("Temperature: ");
  scanf("%f", &ref->t);
  printf("%.1f", ref->t);
  ref->t += 273.0;
  ref->r = getResistance();
  printf(" Resistance: %.0f\n", ref->r);
}

// Main Program
int main() {
  ResTemp ref1, ref2;
  float rx, beta;

  // Init stdio
  stdio_init_all();
  #ifdef LIB_PICO_STDIO_USB
  while (!stdio_usb_connected()) {
      sleep_ms(100);
  }
  #endif

  // Init sensor ADCs
  adc_init();
  adc_gpio_init(THERMISTOR_PIN);
  adc_select_input(THERMISTOR_PIN-26);

  // Gets references
  printf("Reference 1\n");
  getReference (&ref1);
  printf("Reference 2\n");
  getReference (&ref2);

  // Compute beta and rx
  beta = log(ref1.r/ref2.r)/((1/ref1.t)-(1/ref2.t));
  printf("Beta = %.2f\n", beta);
  rx = ref1.r * exp(-beta/ref1.t);

  // Main loop
  while(1) {
    float rt = getResistance();
    float t = beta / log(rt/rx);
    printf("Temperature: %.1f\n", t-273.0);
    sleep_ms(1000);
  }
}
