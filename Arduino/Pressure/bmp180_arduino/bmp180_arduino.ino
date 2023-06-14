// BMP085/BMP180 Pressure Sensor Example
// Adapted from Adafruit Example

#include <Adafruit_BMP085.h>

Adafruit_BMP085 bmp;

// Initialization
void setup() {
  Serial.begin (115200);
  Wire.setSDA(16);
  Wire.setSCL(17);
  if (!bmp.begin()) {
    Serial.println("Could not find a BMP085/BMP180 sensor!");
    while (true) {
    }
  }
}

void loop() {
  Serial.print("Temperature: ");
  Serial.print(bmp.readTemperature());
  Serial.print(" C");
  
  Serial.print("  Pressure: ");
  Serial.print(bmp.readPressure());
  Serial.print(" Pa");
  
  // Calculate altitude assuming 'standard' barometric
  // pressure of 1013.25 millibar = 101325 Pascal
  Serial.print("  Altitude: ");
  Serial.print(bmp.readAltitude());
  Serial.println(" m");

  delay(2000);
}
