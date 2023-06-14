/**
 * @file bme180_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief BMP280/BME280 Sensor Example
 * @version 1.0
 * @date 2023-06-12
 * 
 * A large part of the code was adapted from
 * https://bitbucket.org/christandlg/bmx280mi/src/master/ 
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


// Class to access BME280/BMP280 Sensor
class BMx280 {

  public:

    // oversampling
    typedef enum : uint8_t
    {
      OVSP_SKIP = 0x00,
      OVSP_1    = 0x01,
      OVSP_2    = 0x02,
      OVSP_4    = 0x03,
      OVSP_6    = 0x04,
      OVSP_16   = 0x05
    } OVS_PRESSURE;

    typedef enum : uint8_t
    {
      OVST_SKIP = 0x00,
      OVST_1    = 0x01,
      OVST_2    = 0x02,
      OVST_4    = 0x03,
      OVST_8    = 0x04,
      OVST_16   = 0x05
    } OVS_TEMP;

    typedef enum : uint8_t
    {
      OVSH_SKIP = 0x00,
      OVSH_1    = 0x01,
      OVSH_2    = 0x02,
      OVSH_4    = 0x03,
      OVSH_8    = 0x04,
      OVSH_16   = 0x05
    } OVS_HUMIDITY;

    // filter
    typedef enum : uint8_t
    {
      FILTER_OFF = 0x00,
      FILTER_2   = 0x01,
      FILTER_4   = 0x02,
      FILTER_8   = 0x03,
      FILTER_16  = 0x04,
    } FILTER;

    // modes
    typedef enum : uint8_t
    {
      MODE_SLEEP  = 0x00,
      MODE_FORCED = 0x01,
      MODE_NORMAL = 0x03
    } MODE;

    // standby times
    typedef enum : uint8_t
    {
      TSB_0_5   = 0x00,
      TSB_62_5  = 0x01,
      TSB_125   = 0x02,
      TSB_250   = 0x03,
      TSB_500   = 0x04,
      TSB_1000  = 0x05,
      TSB_10    = 0x06,  // BME280
      TSB_2000  = 0x06,  // BMP280
      TSB_20    = 0x07,  // BME280
      TSB_4000  = 0x07   // BMP280
    } STANDBY;

    static const uint8_t BMP280_ID = 0x58;
	  static const uint8_t BME280_ID = 0x60;

    BMx280 (i2c_inst_t *i2c, uint8_t addr = 0x76,
            OVS_TEMP ovs_t = OVST_16,
            OVS_PRESSURE ovs_p = OVSP_16,
            OVS_HUMIDITY ovs_h = OVSH_16,
            FILTER filter = FILTER_OFF,
            STANDBY stb = TSB_1000
    );

    void softReset();
    uint8_t getDeviceId();
    void setMode(MODE mode);
    void doMeasure(bool bWait);
    bool getRawData();
    float getTemperature();
    float getPressure();
    float getHumidity();
    float calcAltitude(float pressure, float sealevelPressure = 101325.0);

  private:

    // Registers Address
    typedef enum : uint8_t {
      CAL_T1 = 0x88,
      CAL_T2 = 0x8A,
      CAL_T3 = 0x8C,

      CAL_P1 = 0x8E,
      CAL_P2 = 0x90,
      CAL_P3 = 0x92,
      CAL_P4 = 0x94,
      CAL_P5 = 0x96,
      CAL_P6 = 0x98,
      CAL_P7 = 0x9A,
      CAL_P8 = 0x9C,
      CAL_P9 = 0x9E,

      CAL_H1 = 0xA1,
      CAL_H2 = 0xE1,
      CAL_H3 = 0xE3,
      CAL_H4_MSB = 0xE4,
      CAL_H4_H5_LSB = 0xE5,
      CAL_H5_MSB = 0xE6,
      CAL_H6 = 0xE7,

      GET_ID  = 0xD0,
      SOFT_RESET  = 0xE0,
      CTRL_HUM  = 0xF2,
      STATUS  = 0xF3,
      CTRL_MEAS  = 0xF4,
      CONFIG  = 0xF5,
      PRESSURE = 0xF7,
      TEMPERATURE = 0xFA,
      HUMIDITY = 0xFD
    } REGS;
    
    static const uint8_t MASK_MODE = 0x03;
    static const uint8_t SHIFT_MODE = 0;
    static const uint8_t SHIFT_OSRSP = 2;
    static const uint8_t SHIFT_OSRST = 5;
    static const uint8_t SHIFT_STB = 5;
    static const uint8_t SHIFT_FILTER = 2;

    static const uint8_t SOFT_RESET_VALUE = 0xB6;
    static const uint8_t STATUS_UPDATE = 0x01;
    static const uint8_t STATUS_MEASURING = 0x08;

    // I2C instance
    i2c_inst_t *i2c;

    // Sensor I2C Address
    uint8_t addr;

    // Calibration coefficients
    uint16_t  T1 = 0;
    int16_t   T2 = 0;
    int16_t   T3 = 0;

    uint16_t  P1 = 0;
    int16_t   P2 = 0;
    int16_t   P3 = 0;
    int16_t   P4 = 0;
    int16_t   P5 = 0;
    int16_t   P6 = 0;
    int16_t   P7 = 0;
    int16_t   P8 = 0;
    int16_t   P9 = 0;

    uint16_t   H1  = 0;
    int16_t    H2  = 0;
    uint8_t    H3  = 0;
    int16_t    H4  = 0;
    int16_t    H5  = 0;
    int8_t     H6  = 0;

    // Device ID
    uint8_t id;

    // Last read values
    uint16_t uh;
  	uint32_t up;
	  uint32_t ut;    

    // Oversampling
    uint8_t osrs_t;
    uint8_t osrs_p;
    uint8_t osrs_h;

    // Standby time for normal mode
    uint8_t stb_time;

    // Filter
    uint8_t filter;

    // Private routines
    int32_t  calculateTempFine(void);
    void     readCalibCoeff(void);
    uint16_t readRawTemperature(void);
    uint32_t readRawPressure(void);
    void     write8(uint8_t reg, uint8_t val);
    uint8_t  read8(uint8_t reg);
    uint16_t read16(uint8_t reg, bool LoHi = true);
    uint32_t read20 (uint8_t reg);
};

// Constructor
BMx280::BMx280 (i2c_inst_t *i2c, uint8_t addr,
                OVS_TEMP ovs_t, OVS_PRESSURE ovs_p,
                OVS_HUMIDITY ovs_h , FILTER filter,
                STANDBY stb
                ) {
  this->i2c = i2c;
  this->addr = addr;
  this->osrs_t = (uint8_t) ovs_t;
  this->osrs_p = (uint8_t) ovs_p;
  this->osrs_h = (uint8_t) ovs_h;
  this->filter = (uint8_t) filter;
  this->stb_time = (uint8_t) stb;
  this->id = read8(GET_ID);

  // Read calibration coefficients
  readCalibCoeff();

  // Init registers
  write8(CTRL_HUM, osrs_h);
  write8(CONFIG, (stb_time << SHIFT_STB) | (filter << SHIFT_FILTER));
  setMode(MODE_SLEEP);
}

// Do a software reset
void BMx280::softReset () {
  write8(SOFT_RESET, SOFT_RESET_VALUE);
}

// Read Device ID
uint8_t BMx280::getDeviceId() {
  return id;
}

// Change sensor mode
// programs Temperature and Pressure oversampling
void BMx280::setMode(MODE mode) {
  write8 (CTRL_MEAS,
    (osrs_t << SHIFT_OSRST) | (osrs_p << SHIFT_OSRSP) | mode);
}

// Start a measurement
// If bWait = true, waits for it to finish
void BMx280::doMeasure(bool bWait) {
  uint8_t ctrl = read8(CTRL_MEAS);
  uint8_t mode = (ctrl & MASK_MODE) >> SHIFT_MODE;

  if (mode == MODE_SLEEP) {
    // Force a measurement
    setMode (MODE_FORCED);
  }
  if (bWait) {
    sleep_ms(1);  // wait for status to change
    while (!getRawData()) {
      sleep_ms(1);
    }
  }
}

// Get raw data, if available
// return true if data was available
bool BMx280::getRawData() {
  if (read8(STATUS) & STATUS_MEASURING) {
    return false; // measurment not finished
  } else {
    ut = read20(TEMPERATURE);
    up = read20(PRESSURE);
    if (id == BME280_ID) {
      uh = read16(HUMIDITY, false);
    }
    //printf ("UT: %04X UP: %04X UH = %04X\n", ut, up, uh);
    return true;
  }
}

// Get temperature in C
float BMx280::getTemperature() {
  if (getRawData() && (ut != 0x80000)) {
  	return (float) ((calculateTempFine() * 5 + 128) >> 8) / 100.0f;
  } else {
    return NAN;
  }
}

// Get pressure in Pa
float BMx280::getPressure() {
  if (getRawData() && (up != 0x80000)) {
    int32_t temp_fine = calculateTempFine();
    int32_t var1, var2;
    uint32_t p;

    var1 = ((int32_t)temp_fine >> 1) - 64000L;
    var2 = (((var1 >> 2) * (var1 >> 2)) >> 11) * (int32_t) P6;
    var2 = var2 + ((var1*(int32_t)P5) << 1);
    var2 = (var2 >> 2) + ((int32_t)P4 << 16);
    var1 = (((P3 * (((var1 >> 2) * (var1 >> 2)) >> 13)) >> 3) + 
            (((int32_t)P2 * var1) >> 1)) >> 18;
    var1 = ((32768L + var1) * (int32_t)P1) >> 15;

    if (var1 == 0)
      return NAN; // avoid exception caused by division by zero

    p = (((uint32_t)(1048576L - up)) - (var2 >> 12)) * 3125;

    if (p < 0x80000000)
      p = (p << 1) /(uint32_t)var1;
    else
      p = p / (uint32_t)var1 * 2;

    var1 = ((int32_t) P9 * 
            (int32_t) (((p >> 3) * (p >> 3)) >> 13)
           ) >> 12;
    var2 = ((int32_t)(p >> 2) * (int32_t)P8) >> 13;
    p = (uint32_t)((int32_t)p + ((var1 + var2 + P7) >> 4));
    return (float) p;
  } else {
    return NAN;
  }
}

// Get humidity in %
float BMx280::getHumidity() {
  if (id == BME280_ID) {
    if (getRawData() && (uh != 0x8000)) {
      int32_t temp_fine = calculateTempFine();
      int32_t v_x1 = temp_fine - 76800L;
      int32_t v_x2 = ((int32_t) uh << 14) - ((int32_t) H4 << 20) -
                     ((int32_t) H5 * v_x1);
      v_x2 = (v_x2 + 16384L) >> 15;
      int32_t v_x3 = ((v_x1 * (int32_t) H6) >> 10 ) *
                     (((v_x1 * (int32_t) H3) >> 11 ) + 32768L);
      v_x3 = ((v_x3 >> 10) + 2097152L) * (int32_t) H2 + 8192L;
      v_x1 = v_x2 * (v_x3 >> 14);
      v_x1 = v_x1 - ( ((( (v_x1 >> 15) * (v_x1 >> 15) ) >> 7) * (int32_t) H1)
                      >> 4 );
      if (v_x1 < 0) {
        v_x1 = 0;
      } else if (v_x1 > 419430400L) {
        v_x1 = 419430400L;
      }
      return (float) (v_x1 >> 12) / 1024.0f;
    } else {
      return NAN;
    }
  } else {
    return NAN; // No humidity in BMP280
  }
}

// Calculate altitude from pressure
float BMx280::calcAltitude(float pressure, float sealevelPressure) {
  return 44330.0 * (1.0 - pow(pressure/sealevelPressure, 0.1903));
}

// Returns "fine resolution temperature"
// (as defined in section 8.2 of the datasheet)
int32_t BMx280::calculateTempFine() {
	int32_t var1, var2;

  var1 = ((int32_t) ut >> 3) - ((int32_t) T1 << 1);
  var1 = (var1 * (int32_t) T2) >> 11;

  var2 = ((int32_t) ut >> 4) - (int32_t) T1;
  var2 = ((var2 * var2) >> 12) * (int32_t) T3;
	var2 = var2 >> 14;

	return var1 + var2;
}

// Read calibration coefficients and save them
void BMx280::readCalibCoeff() {
  T1 = read16 (CAL_T1);
  T2 = (int16_t) read16 (CAL_T2);
  T3 = (int16_t) read16 (CAL_T3);

  P1 = read16 (CAL_P1);
  P2 = (int16_t) read16 (CAL_P2);
  P3 = (int16_t) read16 (CAL_P3);
  P4 = (int16_t) read16 (CAL_P4);
  P5 = (int16_t) read16 (CAL_P5);
  P6 = (int16_t) read16 (CAL_P6);
  P7 = (int16_t) read16 (CAL_P7);
  P8 = (int16_t) read16 (CAL_P8);
  P9 = (int16_t) read16 (CAL_P9);

  if (id == BME280_ID) {
    H1 = read8(CAL_H1);
    H2 = (int16_t) read16(CAL_H2);
    H3 = read8(CAL_H1);

    uint8_t x = read8(CAL_H4_H5_LSB);
    H4 = (read8(CAL_H4_MSB) << 4) | (x & 0x0F);
    H5 = (read8(CAL_H5_MSB) << 4) | (x >> 4);
    H6 - (int8_t) read8(CAL_H6);
  }
}

// Write an 8-bit value into a register
void BMx280::write8 (uint8_t reg, uint8_t val) {  
  uint8_t buffer[2];

  buffer[0] = reg;
  buffer[1] = val;
  i2c_write_blocking (i2c, addr, buffer, 2, false);
}  
  
// Read a 8-bit value from a register
uint8_t BMx280::read8 (uint8_t reg)  {  
  uint8_t val[1];  
    
  // Select register
  i2c_write_blocking (i2c, addr, &reg, 1, true);

  // Read value
   i2c_read_blocking (i2c, addr, val, 1, false);
  return val[0];
} 

// Read a 16-bit value from a register
uint16_t BMx280::read16 (uint8_t reg, bool LoHi)  {  
  uint8_t val[2];  
    
  // Select register
  i2c_write_blocking (i2c, addr, &reg, 1, true);

  // Read value
   i2c_read_blocking (i2c, addr, val, 2, false);

   if (LoHi) {
    return ((uint16_t) val[1] << 8) | val[0];
  } else {
    return ((uint16_t) val[0] << 8) | val[1];
  }
} 

// Read a 20-bit value from a register
uint32_t BMx280::read20 (uint8_t reg)  {  
  uint8_t val[3];  
    
  // Select register
  i2c_write_blocking (i2c, addr, &reg, 1, true);

  // Read value
   i2c_read_blocking (i2c, addr, val, 3, false);
  return ((uint32_t) val[0] << 12) | 
         ((uint32_t) val[1] << 4) |
         ((uint32_t) val[2] >> 4);  
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
  BMx280 bmp (I2C_ID);

  // Display sensor ID
  uint8_t id = bmp.getDeviceId();
  printf ("Sensor ID = 0x%02X - %s\n", id,
      (id == BMx280::BME280_ID)? "BME280" : "BMP280");

  // Main loop
  while(1) {
    sleep_ms(2000);
    bmp.doMeasure(true);
    float temp = bmp.getTemperature();
    float pressure = bmp.getPressure();
    float altitude = bmp.calcAltitude(pressure);
    printf("Temperature: %.1f C  Pressure: %.0f Pa  Altitude: %.1f m",
      temp, pressure, altitude);
    if (bmp.getDeviceId() == BMx280::BME280_ID) {
      float humidity = bmp.getHumidity();
      printf("  Humidity: %.1f %%", humidity);
    }
    printf("\n");
  }
}
