/**
 * @file bmp180_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief BMP085/BMP180 Sensor Example
 * @version 1.0
 * @date 2023-06-11
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

#define BAUD_RATE     1000000   // 1MHz


// Class to access BMP180 Sensor
class BMP180 {

  public:

    // resolution
    typedef enum : uint8_t
    {
      ULTRALOWPOWER = 0x00,
      STANDARD      = 0x01,
      HIGHRES       = 0x02,
      ULTRAHIGHRES  = 0x03
    } OVERSAMPLING;


    BMP180 (i2c_inst_t *i2c, OVERSAMPLING ovs = ULTRAHIGHRES);
    void softReset();
    uint8_t getDeviceId();
    float getTemperature();
    float getPressure();
    float calcAltitude(float pressure, float sealevelPressure = 101325.0);

  private:
    static const uint8_t BMP180_ADDR = 0x77;
    static const int waitPressure[4];

    // Registers Address
    typedef enum : uint8_t {
      CAL_AC1 = 0xAA,
      CAL_AC2 = 0xAC,
      CAL_AC3 = 0xAE,
      CAL_AC4 = 0xB0,
      CAL_AC5 = 0xB2,
      CAL_AC6 = 0xB4,
      CAL_B1  = 0xB6,
      CAL_B2  = 0xB8,
      CAL_MB  = 0xBA,
      CAL_MC  = 0xBC,
      CAL_MD  = 0xBE,
      GET_ID  = 0xD0,
      SOFT_RESET  = 0xE0,
      START_MEAS  = 0xF4,
      ADC_MSB  = 0xF6,
      ADC_LSB = 0xF7,
      ADC_XLSB = 0xF8
    } REGS;

    static const uint8_t SOFT_RESET_VALUE = 0xB6;
    static const uint8_t GET_TEMP = 0x2E;
    static const uint8_t GET_PRESS = 0x34;

    // I2C instance
    i2c_inst_t *i2c;

    // Calibration coefficients
    int16_t  AC1 = 0;
    int16_t  AC2 = 0;
    int16_t  AC3 = 0;
    uint16_t AC4 = 0;
    uint16_t AC5 = 0;
    uint16_t AC6 = 0;

    int16_t  B1  = 0;
    int16_t  B2  = 0;

    int16_t  MB  = 0;
    int16_t  MC  = 0;
    int16_t  MD  = 0;

    // Oversampling
    uint8_t oss;

    // Private routines
    void     readCalibCoeff(void);
    uint16_t readRawTemperature(void);
    uint32_t readRawPressure(void);
    int32_t  computeB5(int32_t UT);
    void     write8(uint8_t reg, uint8_t val);
    uint8_t  read8(uint8_t reg);
    uint16_t read16(uint8_t reg);
};

// Constructor
BMP180::BMP180 (i2c_inst_t *i2c, OVERSAMPLING ovs) {
  this->i2c = i2c;
  this->oss = (uint8_t) ovs;
  readCalibCoeff();
}

// Do a software reset
void BMP180::softReset () {
  write8(SOFT_RESET, SOFT_RESET_VALUE);
}

// Read Device ID
uint8_t BMP180::getDeviceId() {
  return read8(GET_ID);
}

// Get temperature in C
float BMP180::getTemperature() {
  int16_t rawTemperature = readRawTemperature();
  int32_t B5 = computeB5(rawTemperature);
  return (float)(B5 + 8) / 160.0;
}

// Get pressure in Pa
float BMP180::getPressure() {
  int32_t  UT       = 0;
  int32_t  UP       = 0;
  int32_t  B3       = 0;
  int32_t  B5       = 0;
  int32_t  B6       = 0;
  int32_t  X1       = 0;
  int32_t  X2       = 0;
  int32_t  X3       = 0;
  int32_t  pressure = 0;
  uint32_t B4       = 0;
  uint32_t B7       = 0;

  UT = readRawTemperature();                                            //read uncompensated temperature, 16-bit
  UP = readRawPressure();                                               //read uncompensated pressure, 19-bit
  B5 = computeB5(UT);

  B6 = B5 - 4000;
  X1 = ((int32_t) B2 * ((B6 * B6) >> 12)) >> 11;
  X2 = ((int32_t) AC2 * B6) >> 11;
  X3 = X1 + X2;
  B3 = ((((int32_t) AC1 * 4 + X3) << oss) + 2) / 4;

  X1 = ((int32_t) AC3 * B6) >> 13;
  X2 = ((int32_t) B1 * ((B6 * B6) >> 12)) >> 16;
  X3 = ((X1 + X2) + 2) >> 2;
  B4 = ((uint32_t) AC4 * (X3 + 32768L)) >> 15;
  B7 = (UP - B3) * (50000UL >> oss);
  
  pressure = (B7 < 0x80000000) ? (B7 * 2) / B4 : (B7 / B4) * 2;

  X1 = (pressure >> 8)*(pressure >> 8);
  X1 = (X1 * 3038L) >> 16;
  X2 = (-7357L * pressure) >> 16;

  return pressure + ((X1 + X2 + 3791L) >> 4);
}

// Calculate altitude from pressure
float BMP180::calcAltitude(float pressure, float sealevelPressure) {
  return 44330.0 * (1.0 - pow(pressure/sealevelPressure, 0.1903));
}

// Read calibration coefficients and save them
void BMP180::readCalibCoeff() {
  AC1 = read16 (CAL_AC1);
  AC2 = read16 (CAL_AC2);
  AC3 = read16 (CAL_AC3);
  AC4 = read16 (CAL_AC4);
  AC5 = read16 (CAL_AC5);
  AC6 = read16 (CAL_AC6);
  B1 = read16 (CAL_B1);
  B2 = read16 (CAL_B2);
  MB = read16 (CAL_MB);
  MC = read16 (CAL_MC);
  MD = read16 (CAL_MD);
}

// Calculate B5 parameter
int32_t BMP180::computeB5(int32_t UT)
{
  int32_t X1 = ((UT - (int32_t) AC6) * (int32_t) AC5) >> 15;
  int32_t X2 = ((int32_t) MC << 11) / (X1 + (int32_t) MD);

  return X1 + X2;
}

// Read raw temperature data
uint16_t BMP180::readRawTemperature(void)
{
  write8(START_MEAS, GET_TEMP);
  sleep_ms(5);
  return read16(ADC_MSB);
}

// Read raw pressure data
const int BMP180::waitPressure[4] = { 5, 8, 14, 26 };
uint32_t BMP180::readRawPressure(void)
{
  write8(START_MEAS, GET_PRESS + (oss << 6));
  sleep_ms(waitPressure[oss]);
  uint32_t rawPressure = read16(ADC_MSB) << 8;
  rawPressure |= read8(ADC_XLSB);
  rawPressure >>= (8 - oss);
  return rawPressure;
}

// Write an 8-bit value into a register
void BMP180::write8 (uint8_t reg, uint8_t val) {  
  uint8_t buffer[2];

  buffer[0] = reg;
  buffer[1] = val;
  i2c_write_blocking (i2c, BMP180_ADDR, buffer, 2, false);
}  
  
// Read a 8-bit value from a register
uint8_t BMP180::read8 (uint8_t reg)  {  
  uint8_t val[1];  
    
  // Select register
  i2c_write_blocking (i2c, BMP180_ADDR, &reg, 1, true);

  // Read value
   i2c_read_blocking (i2c, BMP180_ADDR, val, 1, false);
  return val[0];
} 

// Read a 16-bit value from a register
uint16_t BMP180::read16 (uint8_t reg)  {  
  uint8_t val[2];  
    
  // Select register
  i2c_write_blocking (i2c, BMP180_ADDR, &reg, 1, true);

  // Read value
   i2c_read_blocking (i2c, BMP180_ADDR, val, 2, false);
  return ((uint16_t) val[0] << 8) | val[1];  
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
  BMP180 bmp180 (I2C_ID);

  // Display sensor ID
  printf ("Sensor ID = 0x%02X\n", bmp180.getDeviceId());

  // Main loop
  while(1) {
    sleep_ms(2000);
    float temp = bmp180.getTemperature();
    float pressure = bmp180.getPressure();
    float altitude = bmp180.calcAltitude(pressure);
    printf("Temperature: %.1f C  Pressure: %.0f Pa  Altitude: %.1f m\n",
      temp, pressure, altitude);  
  }
}
