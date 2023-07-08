// MMA8452 Sensor Example

#include <Wire.h>
#include "SparkFun_MMA8452Q.h"

MMA8452Q sensor;

// Initialization
void setup() {
  while(!Serial) {
    delay(100);
  }
  Serial.begin(115200);
  Wire.setSDA(16);
  Wire.setSCL(17);
  Wire.begin();
  sensor.begin(Wire, 0x1C);
  Serial.println(sensor.readID());
}

// Main Loop
void loop() {
  if (sensor.available()) {
    Serial.print ("Accel X:");
    Serial.print (sensor.getCalculatedX(), 1);
    Serial.print ("g Y:");
    Serial.print (sensor.getCalculatedY(), 1);
    Serial.print ("g Z:");
    Serial.print (sensor.getCalculatedZ(), 1);
    Serial.println ("g");
    delay(2000);
  }
}
