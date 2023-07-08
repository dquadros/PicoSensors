/**
 * @file mma8452_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief MMA8452 Sensor Example
 * @version 1.0
 * @date 2023-07-06
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

// Class to access sensor
class MMA8452 {

  public:

    typedef struct {
      float x;
      float y;
      float z;
    } VECT_3D;

    // Default I2C Address
    static const uint8_t I2C_ADDR = 0x1C;

    // Chip ID
    static const uint8_t ID = 42;

    // Constructor
    MMA8452 (i2c_inst_t *i2c, uint8_t addr = MMA8452::I2C_ADDR);

    // Public methods
    void    begin(void);
    uint8_t getId(void);
    void    getAccel(VECT_3D *vect, float scale = 1024.0);
    void    active(void);
    void    standby(void);

  private:
    
    // Registers Address
    static const uint8_t XYZ_DATA_CFG = 0x0E;
    static const uint8_t WHO_AM_I = 0x0D;
    static const uint8_t CTRL_REG1 = 0x2A;
    static const uint8_t PULSE_CFG = 0x21;
    static const uint8_t OUT_X_MSB = 0x01;

    // I2C instance
    i2c_inst_t *i2c;

    // Sensor I2C Address
    uint8_t addr;

    // Sensor ID
    uint8_t id;

    // Raw data
    int16_t raw[14];

    // Private rotines
    void     readRaw(void);
    uint8_t  read8(uint8_t reg);
    void     write8(uint8_t reg, uint8_t val);
};

// Constructor
MMA8452::MMA8452 (i2c_inst_t *i2c, uint8_t addr) {
  this->i2c = i2c;
  this->addr = addr;
}

// Set up sensor
void MMA8452::begin() {
  this->id = read8(WHO_AM_I);
  standby();
  uint8_t val = read8(XYZ_DATA_CFG);
  write8(XYZ_DATA_CFG, val& 0xFC);  // scale = 2g
  val = read8(CTRL_REG1);
  write8(CTRL_REG1, val & 0xC7);  // odr = 500Hz
  write8(PULSE_CFG, 0x00);  // disable tap detection
  active();
}

// Returns the sensor id
uint8_t MMA8452::getId() {
  return id;
}

// Go into active mode
void MMA8452::active() {
  uint8_t val = read8(CTRL_REG1);
  write8(CTRL_REG1, val | 0x01);
}

// Go into standby mode
void MMA8452::standby() {
  uint8_t val = read8(CTRL_REG1);
  write8(CTRL_REG1, val & 0xFE);
}


// Get acceleration in g
void MMA8452::getAccel(VECT_3D *vect, float scale) {
  readRaw();
  vect->x = (float) raw[0] / scale;
  vect->y = (float) raw[1] / scale;
  vect->z = (float) raw[2] / scale;
}

// Read raw values
void MMA8452::readRaw() {
  uint8_t data[6];

  // Select first register
  uint8_t reg = OUT_X_MSB;
  i2c_write_blocking (i2c, addr, &reg, 1, true);

  // Read values
  i2c_read_blocking (i2c, addr, data, 6, false);

  // Convert to int16
  for (int i = 0; i < 3; i++) {
    raw[i] = (data[2*i] << 8) | data[2*i+1];
    raw[i] /= 16;
  }
}

// Read a 8-bit value from a register
uint8_t MMA8452::read8 (uint8_t reg)  {  
  uint8_t val[1];  
    
  // Select register
  i2c_write_blocking (i2c, addr, &reg, 1, true);

  // Read value
  i2c_read_blocking (i2c, addr, val, 1, false);
  return val[0];
} 

// Write a 8-bit value to a register
void MMA8452::write8 (uint8_t reg, uint8_t val)  {  
  uint8_t aux[2];

  aux[0] = reg;
  aux[1] = val;
  i2c_write_blocking (i2c, addr, aux, 2, false);
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
  MMA8452 sensor (I2C_ID);
  sensor.begin();

  // Display sensor ID
  printf ("Sensor ID: %02X\n", sensor.getId());

  // Main loop
  MMA8452::VECT_3D data;
  while(1) {
    sleep_ms(2000);
    sensor.getAccel(&data);
    printf ("Accel X:%.1f Y:%.1f Z:%.1f\n", data.x, data.y, data.z);
  }
}
