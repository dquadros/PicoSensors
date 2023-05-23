// MCP9808 Temperature Sensor Example
  
#include <Wire.h>  
  
// MCP9808 I2C Address 
#define ADDR      0x18  
  
// MCP9808 Registers
#define REG_CONFIG  1
#define REG_UPPER   2  
#define REG_LOWER   3
#define REG_CRIT    4
#define REG_TA      5
#define REG_MANID   6
#define REG_DEVID   7
#define REG_RESOL   8
  
// Initialization
void setup() {  
  Serial.begin (115200);
  Wire.setSDA(16);
  Wire.setSCL(17);
  Wire.begin();  
  
  // Check manufacture and device IDs
  uint16_t manID = ReadReg16(REG_MANID);
  uint16_t devID = ReadReg16(REG_DEVID);
  Serial.print ("Manufacturer: ");
  Serial.println (manID, HEX);
  Serial.print ("Device: ");
  Serial.print (devID >> 8, HEX);
  Serial.print (" rev ");
  Serial.println (devID & 0xFF);

  // Set limits for the Alert output
  WriteReg16(REG_CRIT,EncodeTemp(30.0));
  WriteReg16(REG_UPPER,EncodeTemp(23.0));
  WriteReg16(REG_LOWER,EncodeTemp(20.0));
  WriteReg16(REG_CONFIG,0x0008);
}  
  
// Main loop: read temperature
void loop() {  
  delay (500);
  Serial.print("Temperature: ");
  Serial.print(DecodeTemp(ReadReg16(REG_TA)));  
  Serial.println("C");
}  
  
// Encode temperature for sensor
int16_t EncodeTemp (float temp)  
{
  if (temp >= 0) {
    return ((uint16_t) (temp/0.25)) << 2;
  } else {
    return ((uint16_t) (2048 + temp/0.25)) << 2;
  }  
}  
  
// Decode temperature from sensor
float DecodeTemp (int16_t val)  
{  
  bool sign = (val & 0x1000) != 0;
  val = val & 0x1FFF;
  if (sign) {
    val = val - 0x2000;
  }
  return (float)val/16.0;
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