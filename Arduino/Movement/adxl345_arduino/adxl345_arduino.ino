// ADXL345 Sensor Example

#include "Wire.h"
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>

Adafruit_ADXL345_Unified sensor = Adafruit_ADXL345_Unified(1);

const float G = 9.8;  // to convert m/s^2 to g

// Initialization
void setup() {
  while(!Serial) {
    delay(100);
  }
  Serial.begin(115200);
  Wire.setSDA(16);
  Wire.setSCL(17);
  sensor.begin();
  sensor.setRange(ADXL345_RANGE_2_G);
}

// Main Loop
void loop() {
  sensors_event_t event; 
  sensor.getEvent(&event);

  Serial.print ("Accel X:");
  Serial.print (event.acceleration.x/G, 1);
  Serial.print ("g Y:");
  Serial.print (event.acceleration.y/G, 1);
  Serial.print ("g Z:");
  Serial.print (event.acceleration.z/G, 1);
  Serial.println ("g");

  delay(2000);
}
