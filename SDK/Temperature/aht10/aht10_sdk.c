/**
 * @file aht10_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief AHT10 Temperature Sensor Example
 * @version 1.0
 * @date 2023-06-07
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Sensor Connections
#define I2C_ID        i2c0
#define I2C_SCL_PIN   17
#define I2C_SDA_PIN   16

#define BAUD_RATE     400000   // fast-mode 400KHz

// AHT10 I2C Address
#define AHT10_ADDR   0x38  

// AHT10 Commands
uint8_t cmdInit[] = { 0xE1, 0x08, 0x00 };
uint8_t cmdConv[] = { 0xAC, 0x33, 0x00 };

// Read Status
int8_t getStatus ()  {  
  uint8_t val[1];  
    
  i2c_read_blocking (I2C_ID, AHT10_ADDR, val, 1, false);
  return val[0];  
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

  // Init I2C interface
  uint baud = i2c_init (I2C_ID, BAUD_RATE);
  printf ("I2C @ %u Hz\n", baud);
  gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SCL_PIN);
  gpio_pull_up(I2C_SDA_PIN);

  // Check if a calibration is needed
  uint8_t status = getStatus();
  if ((status & 0x08) == 0) {
    printf ("Calibrating\n");
    i2c_write_blocking (I2C_ID, AHT10_ADDR, cmdInit, 
                        sizeof(cmdInit), false);
    sleep_ms(10);
  }    

  // Main Loop
  while(1) {
    // Starts a conversion
    i2c_write_blocking (I2C_ID, AHT10_ADDR, cmdConv, 
                        sizeof(cmdConv), false);
    // Wait conversion
    sleep_ms(80);
    // Get and show result
    uint8_t r[6];
    i2c_read_blocking (I2C_ID, AHT10_ADDR, r, sizeof(r), false);
    float humid = (r[1] << 12) + (r[2] << 4) + (r[3] >> 4);
    humid = (humid / 0x100000) * 100.0;
    float temp = ((r[3] & 0x0F) << 16) + (r[4] << 8) + r[5];
    temp = (temp / 0x100000) * 200.0 - 50.0;
    printf("Temperature: %.1fC Humidity: %.0f%%\n", temp, humid);  
    sleep_ms(2000);
  }
}
