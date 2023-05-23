/**
 * @file lm75a_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief LM75A Temperature Sensor Example
 * @version 1.0
 * @date 2023-05-21
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

// LM75A I2C Address 
#define LM75A_ADDR    0x48  
  
// LM75A Registers
#define REG_TEMP  0  
#define REG_CONF  1  
#define REG_THYST 2  
#define REG_TOS   3  
  
//Write an 8-bit value into a register
void WriteReg8 (uint8_t reg, int8_t val) {  
  uint8_t buffer[2];

  buffer[0] = reg;
  buffer[1] = val;
  i2c_write_blocking (I2C_ID, LM75A_ADDR, buffer, 2, false);
}  
  
//Write a 16-bit value into a register
void WriteReg16 (uint8_t reg, int16_t val)  {  
  uint8_t buffer[3];

  buffer[0] = reg;
  buffer[1] = val >> 8;
  buffer[2] = val & 0xFF;
  i2c_write_blocking (I2C_ID, LM75A_ADDR, buffer, 3, false);
}  
  
// Read a 16-bit value from a register
int16_t ReadReg16 (uint8_t reg)  {  
  uint8_t val[2];  
    
  // Select register
  i2c_write_blocking (I2C_ID, LM75A_ADDR, &reg, 1, true);

  // Read value
   i2c_read_blocking (I2C_ID, LM75A_ADDR, val, 2, false);
  return ((int16_t) val[0] << 8) | val[1];  
} 

// Encode temperature for sensor
int16_t EncodeTemp (float temp)  {  
  if (temp >= 0) {
    return ((int16_t) (temp / 0.5)) << 7;
  }  else {
    return (512 + (int16_t) (temp / 0.5)) << 7;
  }
}  
  
// Decode temperature from sensor
float DecodeTemp (int16_t val)  {  
  val = val / 32;
  if (val >= 1024) {
    return ((float) (val-2048)) * 0.125;
  } else {
    return ((float) val) * 0.125;
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

  // Init I2C interface
  uint baud = i2c_init (I2C_ID, BAUD_RATE);
  printf ("I2C @ %u Hz\n", baud);
  gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
  gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
  gpio_pull_up(I2C_SCL_PIN);
  gpio_pull_up(I2C_SDA_PIN);

  // Configure sensor and set limits for the OS output  
  WriteReg8 (REG_CONF, 0);
  WriteReg16 (REG_TOS, EncodeTemp(22.5));  
  WriteReg16 (REG_THYST, EncodeTemp(20.0));  

  // Main loop
  while(1) {
    sleep_ms(500);
    printf("Temperature: %.3fC\n", DecodeTemp(ReadReg16(REG_TEMP)));  
  }
}
