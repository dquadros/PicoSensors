/**
 * @file ds18b20_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief DS18B20 Temperature Sensor Example
 * @version 1.0
 * @date 2023-05-19
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico-onewire/api/one_wire.h"

// Pins
#define SENSOR_PIN 16

static One_wire one_wire(SENSOR_PIN);

#define MAX_SENSORS 5
static int nSensors;
static rom_address_t sensor[MAX_SENSORS];

// Main Program
int main() {
  // Init stdio
  stdio_init_all();
  #ifdef LIB_PICO_STDIO_USB
  while (!stdio_usb_connected()) {
      sleep_ms(100);
  }
  #endif

  // Find sensors
	one_wire.init();
	int count = one_wire.find_and_count_devices_on_bus();
	nSensors = 0;
	for (int i = 0; i < count; i++) {
		auto address = One_wire::get_address(i);
		if ((address.rom[0] == FAMILY_CODE_DS18B20) && (nSensors < MAX_SENSORS)) {
			sensor[nSensors] = address;
			nSensors++;
		}
	}

  // Main loop
  while(1) {
    // Start conversion
    for (int i = 0; i < nSensors; i++) {
      one_wire.convert_temperature(sensor[i], i == (nSensors-1), false);
  	}

    // Get results and print
    for (int i = 0; i < nSensors; i++) {
      auto address = sensor[i];
      float temp = one_wire.temperature(address);
      printf("%02x%02x%02x%02x%02x%02x%02x%02x ", address.rom[0], 
          address.rom[1], address.rom[2], address.rom[3], address.rom[4], 
          address.rom[5], address.rom[6], address.rom[7]);
      printf(" %3.1fC\n", temp);
    }

    printf("\n");
    sleep_ms(250);
  }
}
