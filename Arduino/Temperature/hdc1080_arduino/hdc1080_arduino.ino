// HDC1080 Temperature Sensor Example
  
#include <Wire.h>  
  
// HDC1080 I2C Address 
#define ADDR      0x40  
  
// HDC1080 Registers
#define REG_TEMP  0  
#define REG_HUM   1  
#define REG_CONF  2  
#define REG_MANID 0xFE
#define REG_DEVID 0xFF
  
// Initialization
void setup() {  
  Serial.begin (115200);
  Wire.setSDA(16);
  Wire.setSCL(17);
  Wire.begin();  

  delay(5000);
  
  // Check manufacture and device IDs
  uint16_t manID = ReadReg16(REG_MANID);
  uint16_t devID = ReadReg16(REG_DEVID);
  Serial.print ("Manufacturer: ");
  Serial.print (manID, HEX);
  Serial.print (" Device: ");
  Serial.println (devID, HEX);
}  
  
// Main loop: read temperature
void loop() {  
  // Start conversion
  Wire.beginTransmission(ADDR);  
  Wire.write(REG_TEMP);  
  Wire.endTransmission();

  // Wait conversion
  delay(20);

  // Get result and print
  uint16_t r;
  float temp, humid;
  Wire.requestFrom(ADDR, 4);
  r = Wire.read() << 8;
  r |= Wire.read();  
  temp = r*165.0/65536.0 - 40.0;
  r = Wire.read() << 8;
  r |= Wire.read();  
  humid = r*100.0/65536.0;
  Serial.print("Temperature: ");
  Serial.print(temp, 1);  
  Serial.print("C Humidity: ");
  Serial.print(humid, 0);  
  Serial.println("\%");  
  delay (2000);
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