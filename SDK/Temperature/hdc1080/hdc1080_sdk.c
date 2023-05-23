/**
 * @file hdc1080_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief HDC1080 Temperature Sensor Example
 * @version 1.0
 * @date 2023-05-22
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"

// Sensor connections
#define I2C_ID        i2c0
#define I2C_SCL_PIN   17
#define I2C_SDA_PIN   16

#define BAUD_RATE     400000   // fast-mode 400KHz

// HDC1080 I2C Address 
#define HDC1080_ADDR   0x40  
  
// HDC1080 Registers
#define REG_TEMP  0  
#define REG_HUM   1  
#define REG_CONF  2  
#define REG_MANID 0xFE
#define REG_DEVID 0xFF
  
// Read a 16-bit value from a register
int16_t ReadReg16 (uint8_t reg)  {  
  uint8_t val[2];  
    
  // Select register
  i2c_write_blocking (I2C_ID, HDC1080_ADDR, &reg, 1, true);

  // Read value
   i2c_read_blocking (I2C_ID, HDC1080_ADDR, val, 2, false);
  return ((int16_t) val[0] << 8) | val[1];  
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

  // Check manufacture and device IDs
  uint16_t manID = ReadReg16(REG_MANID);
  uint16_t devID = ReadReg16(REG_DEVID);
  printf ("Manufacturer: %04X Device: %04X\n",
    manID, devID);

  // Main loop
  uint8_t reg[] = { REG_TEMP };
  uint8_t val[4];
  while(1) {
    // Start conversion
    i2c_write_blocking (I2C_ID, HDC1080_ADDR, &reg, 1, true);
    // Wait conversion
    sleep_ms(20);
    // Get result and print
    i2c_read_blocking (I2C_ID, HDC1080_ADDR, val, 4, false);
    uint16_t r = ((int16_t) val[0] << 8) | val[1];
    float  temp = r*165.0/65536.0 - 40.0;
    r = ((int16_t) val[2] << 8) | val[3];
    float humid = r*100.0/65536.0;
    printf("Temperature: %.1fC Humidity: %.0f%%\n", temp, humid);  
    sleep_ms(2000);
  }
}
