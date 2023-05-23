/**
 * @file mcp9808_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief MCP9808 Temperature Sensor Example
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

// MCP9808 I2C Address 
#define MCP9808_ADDR   0x18  
  
// MCP9808 Registers
#define REG_CONFIG  1
#define REG_UPPER   2  
#define REG_LOWER   3
#define REG_CRIT    4
#define REG_TA      5
#define REG_MANID   6
#define REG_DEVID   7
#define REG_RESOL   8
  
//Write a 16-bit value into a register
void WriteReg16 (uint8_t reg, int16_t val)  {  
  uint8_t buffer[3];

  buffer[0] = reg;
  buffer[1] = val >> 8;
  buffer[2] = val & 0xFF;
  i2c_write_blocking (I2C_ID, MCP9808_ADDR, buffer, 3, false);
}  
  
// Read a 16-bit value from a register
int16_t ReadReg16 (uint8_t reg)  {  
  uint8_t val[2];  
    
  // Select register
  i2c_write_blocking (I2C_ID, MCP9808_ADDR, &reg, 1, true);

  // Read value
   i2c_read_blocking (I2C_ID, MCP9808_ADDR, val, 2, false);
  return ((int16_t) val[0] << 8) | val[1];  
} 

// Encode temperature for sensor
int16_t EncodeTemp (float temp)  {  
  if (temp >= 0) {
    return ((uint16_t) (temp/0.25)) << 2;
  } else {
    return ((uint16_t) (2048 + temp/0.25)) << 2;
  }  
}  
  
// Decode temperature from sensor
float DecodeTemp (int16_t val)  {  
  bool sign = (val & 0x1000) != 0;
  val = val & 0x1FFF;
  if (sign) {
    val = val - 0x2000;
  }
  return (float)val/16.0;
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
  printf ("Manufacturer: %04X Device: %02X rev %u\n",
    manID, devID >> 8, devID & 0xFF);

  // Set limits for the Alert output
  WriteReg16(REG_CRIT,EncodeTemp(30.0));
  WriteReg16(REG_UPPER,EncodeTemp(23.0));
  WriteReg16(REG_LOWER,EncodeTemp(20.0));
  WriteReg16(REG_CONFIG,0x0008);

  // Main loop
  while(1) {
    sleep_ms(500);
    printf("Temperature: %.3fC\n", DecodeTemp(ReadReg16(REG_TA)));  
  }
}
