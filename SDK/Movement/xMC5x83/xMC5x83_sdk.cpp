/**
 * @file xMC5x83_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief HMC5883L/HMC5983/QMC5883L Sensor Example
 * @version 1.0
 * @date 2023-07-02
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

#define BAUD_RATE     400000   // 400kHz

const float PI = 3.14159f;

// Class to access sensor
class COMPASS {

  public:

    // Sensor model
    typedef enum { UNDEFINED, HMC_58, HMC_59, QMC } MODEL;

    // Constructor
    COMPASS (i2c_inst_t *i2c, MODEL model = UNDEFINED);

    // Public methods
    bool  begin(void);
    float getHeading(void);
    MODEL getModel(void);

  private:
    
    // I2C Addresses
    static const uint8_t ender_HMC = 0x1E;
    static const uint8_t ender_QMC = 0x0D;

    // Registers Address
    static const int regCFGA_HMC = 0;
    static const int regCFGB_HMC = 1;
    static const int regMODE_HMC = 2;
    static const int regXH_HMC = 3;
    static const int regST_HMC = 9;
    
    static const int regCR1_QMC = 9;
    static const int regSR_QMC = 11;
    static const int regXL_QMC = 0;
    static const int regST_QMC = 6;

    // I2C instance
    i2c_inst_t *i2c;

    // Sensor I2C Address
    uint8_t addr;

    // Sensor model
    MODEL model;

    // Private rotines
    bool     checkAddr(uint8_t addr);
    void     write8(uint8_t reg, uint8_t val);
    uint8_t  read8(uint8_t reg);
};

// Constructor
COMPASS::COMPASS (i2c_inst_t *i2c, MODEL model) {

  this->i2c = i2c;
  this->model = model;
}

// Set up sensor
bool COMPASS::begin() {

  // Set up model and i2c address
  if (model == UNDEFINED) {
    // Try to identify chip
    if (checkAddr(ender_QMC)) {
      this->addr = ender_QMC;
      this->model = QMC;
    } else if (checkAddr(ender_HMC)) {
      this->addr = ender_HMC;
      // Test if we can set temperature compensation
      write8(regCFGA_HMC, 0x90);
      this->model = (read8(regCFGA_HMC) == 0x90)? HMC_59 : HMC_58;
    } else {
      return false; // sensor not faound
    }
  } else {
    this->model = model;
    this->addr = (model == QMC) ? ender_QMC: ender_HMC;
  }

  // init registers for continous mode
  if (model == QMC) {
    write8(regSR_QMC, 0x01);  // as per datasheet
    write8(regCR1_QMC, 0x81); // 10Hz, 2G, 512 osr
  } else if (model == HMC_58) {
    // default values as I could not find a HMC5883L to test
    write8(regCFGA_HMC, 0x10);
    write8(regCFGB_HMC, 0x01);
    write8(regMODE_HMC, 0x00);
  } else {
    // crank up gain and samples
    write8(regCFGA_HMC, 0xF0);
    write8(regCFGB_HMC, 0x00);
    write8(regMODE_HMC, 0x00);
  }

  return true;
}

// Returns the sensor model
COMPASS::MODEL COMPASS::getModel() {
  return model;
}

// Get the compass heading (0 to 360 degrees)
float COMPASS::getHeading() {
  uint8_t data[6];
  uint8_t reg[1];
  int16_t x,y,z;


  if (model == UNDEFINED) {
    return 0.0f;
  }

  if (model == QMC) {
    // wait for data
    while ((read8(regST_QMC) & 0x01) == 0)
      ;
    // read data
    reg[0] = regXL_QMC;
    i2c_write_blocking (i2c, addr, reg, 1, true);
    i2c_read_blocking (i2c, addr, data, 6, false);
    // get 16-bit info
    x = data[0] + (data[1] << 8);
    y = data[2] + (data[3] << 8);
    z = data[4] + (data[5] << 8);
  } else {
    // wait for data
    while ((read8(regST_HMC) & 0x01) == 0)
      ;
    // read data
    reg[0] = regXH_HMC;
    i2c_write_blocking (i2c, addr, reg, 1, true);
    i2c_read_blocking (i2c, addr, data, 6, false);
    // get 16-bit info
    x = data[1] + (data[0] << 8);
    z = data[3] + (data[2] << 8);
    y = data[5] + (data[4] << 8);
  }

  // compute heading
  float angle = atan2((float)y,(float)x);
  //printf ("%8d %8d %8d %.2f\n", x, y, z, angle/PI);
  return (angle*180.0f)/PI + 180.0f;
}

// Check if there is something at an I2C address
bool COMPASS::checkAddr(uint8_t addr) {
  int ret;
  uint8_t rxdata;

  ret = i2c_read_blocking(this->i2c, addr, &rxdata, 1, false);
  return ret >= 0;
}

// Write an 8-bit value into a register
void COMPASS::write8 (uint8_t reg, uint8_t val) {  
  uint8_t buffer[2];

  buffer[0] = reg;
  buffer[1] = val;
  i2c_write_blocking (i2c, addr, buffer, 2, false);
}  
  
// Read a 8-bit value from a register
uint8_t COMPASS::read8 (uint8_t reg)  {  
  uint8_t val[1];  
    
  // Select register
  i2c_write_blocking (i2c, addr, &reg, 1, true);

  // Read value
  i2c_read_blocking (i2c, addr, val, 1, false);
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

  // Init sensor
  COMPASS compass (I2C_ID);
  if (!compass.begin()) {
    printf ("Sensor not found!");
    while (true) {
      sleep_ms(100);
    }
  }

  // Display sensor model
  COMPASS::MODEL model = compass.getModel();
  printf ("Sensor Model = %s\n", 
    (model == COMPASS::QMC) ? "QMC5883L" :
    (model == COMPASS::HMC_58) ? "HMC5883L" : "HMC5983"
  );

  // Main loop
  while(1) {
    sleep_ms(2000);
    printf ("Heading: %.1f\n", compass.getHeading());
  }
}
