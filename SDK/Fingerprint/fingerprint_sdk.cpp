/**
 * @file fingerprint_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Fingerprint Sensor Example
 * @version 1.0
 * @date 2023-08-22
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "fpm10a_sdk.h"

// Sensor connections
#define UART_ID       uart0
#define UART_RX_PIN   17
#define UART_TX_PIN   16

// RGB LED connections
#define LED_R_PIN     13
#define LED_G_PIN     14
#define LED_B_PIN     15
#define LED_MASK      ((1 << LED_R_PIN) | (1 << LED_G_PIN) | (1 << LED_B_PIN))

FPM10A *sensor;

// Read fingerprint and create template
void captureFeature(uint8_t numbuf) {
  while (true) {
    gpio_put (LED_B_PIN, true);
    printf ("Place finger on sensor\n");
    while (!sensor->capture()) {
      sleep_ms(10);
    }
    gpio_put (LED_B_PIN, false);
    printf("Image captured\n");

    bool ok = sensor->createFeature(numbuf);
    printf ("Remove finger from sensor\n");
    sleep_ms(2000);
    if (ok) {
      printf ("Feature created\n");
      return;
    }
    gpio_put (LED_R_PIN, true);
    printf ("Bad image, try again\n");
    sleep_ms(1000);
    gpio_put (LED_R_PIN, false);
  }
}

// Enroll fingerprint
void enroll() {
  bool first = true;
  while (true) {
    if (!first) {
      captureFeature(1);
    }
    captureFeature(2);
    if (!sensor->createModel()) {
      gpio_put (LED_R_PIN, true);
      printf ("Images do not match, try again\n");
      sleep_ms(1000);
      gpio_put (LED_R_PIN, false);
      continue;
    }
    int pos = sensor->count();
    if (sensor->store(1, pos)) {
      gpio_put (LED_G_PIN, true);
      printf ("Fingerprint stored at %d\n", pos);
      sleep_ms(1000);
      gpio_put (LED_G_PIN, false);
      return;
    }
    gpio_put (LED_R_PIN, true);
    printf ("Error %d while saving fingerprint\n", sensor->lastResponse());
    sleep_ms(1000);
    gpio_put (LED_R_PIN, false);
  }
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
  printf ("\nFingerprint Sensor Example\n\n");

  // Init LED
  gpio_init_mask (LED_MASK);
  gpio_set_dir_masked (LED_MASK, LED_MASK);
  gpio_put_masked (LED_MASK, 0);

  // Init serial interface
  gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
  gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);

  // Init sensor
  sensor = new FPM10A(UART_ID);
  sensor->begin();

  // Show how many fingerprints can be stores
  printf ("Checking sensor capacity\n");
  FPM10A::SYSPARAM sp;
  if (sensor->leSysParam(&sp)) {
    printf ("Sensor can store %d fingerprint\n", 
      (sp.libsize[0] << 8) + sp.libsize[1]);
  }

  // Clear all stored fingerprints
  int count = sensor->count();
  if (count > 0) {
    printf ("Erasing %d fingerprints\n", count);
    if (sensor->clear()) {
      printf ("Success\n");
    }
  }

  // Main loop
  while(1) {
    printf("\n");
    captureFeature(1);
    printf("Searching...\n");
    int pos = sensor->search(1);
    if (pos == -1) {
      printf ("Unknown, let\'s enroll\n");
      enroll();
    } else {
      gpio_put (LED_G_PIN, true);
      printf ("Fingerprint %d identified\n", pos);
      sleep_ms(1000);
      gpio_put (LED_G_PIN, false);
    }
  }
}
