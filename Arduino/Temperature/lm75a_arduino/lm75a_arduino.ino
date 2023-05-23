// LM75A Temperature Sensor Example
  
#include <Wire.h>  
  
// LM75A I2C Address 
#define ADDR      0x48  
  
// LM75A Registers
#define REG_TEMP  0  
#define REG_CONF  1  
#define REG_THYST 2  
#define REG_TOS   3  
  
// Initialization
void setup() {  
  Serial.begin (115200);
  Wire.setSDA(16);
  Wire.setSCL(17);
  Wire.begin();  
  
  // Configure sensor and set limits for the OS output  
  WriteReg8 (REG_CONF, 0);
  WriteReg16 (REG_TOS, EncodeTemp(22.5));  
  WriteReg16 (REG_THYST, EncodeTemp(20.0));  
}  
  
// Main loop: read temperature
void loop() {  
  delay (500);
  Serial.print("Temperature: ");
  Serial.print(DecodeTemp(ReadReg16(REG_TEMP)));  
  Serial.println("C");
}  
  
// Encode temperature for sensor
int16_t EncodeTemp (float temp)  
{
  if (temp >= 0) {
    return ((int16_t) (temp / 0.5)) << 7;
  }  else {
    return (512 + (int16_t) (temp / 0.5)) << 7;
  }
}
  
// Decode temperature from sensor
float DecodeTemp (int16_t val)  
{  
  val = val / 32;
  if (val >= 1024) {
    return ((float) (val-2048)) * 0.125;
  } else {
    return ((float) val) * 0.125;
  }
}  
  
//Write an 8-bit value into a register
void WriteReg8 (byte reg, int8_t val)  
{  
  Wire.beginTransmission(ADDR);  
  Wire.write(reg);  
  Wire.write(val);  
  Wire.endTransmission();  
}  
  
//Write a 16-bit value into a register
void WriteReg16 (byte reg, int16_t val)  
{  
  Wire.beginTransmission(ADDR);  
  Wire.write(reg);  
  Wire.write((val >> 8) & 0xFF);  
  Wire.write(val & 0xFF);  
  Wire.endTransmission();  
}  
  
// Read a 16-bit value from a register
int16_t ReadReg16 (byte reg)  
{  
  uint16_t val;  
    
  // Select register
  Wire.beginTransmission(ADDR);  
  Wire.write(reg);  
  Wire.endTransmission();  
  
  // Read value
  Wire.requestFrom(ADDR, 2);  
  val = Wire.read() << 8;  
  val |= Wire.read();  
  return (int16_t) val;  
} 