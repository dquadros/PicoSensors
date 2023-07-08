// MPU6050 Sensor Example

#include "Wire.h"
#include <MPU6050_light.h>

MPU6050 sensor(Wire);
long nextReading = 0L;

// Initialization
void setup() {
  while(!Serial) {
    delay(100);
  }
  Serial.begin(115200);
  Wire.setSDA(16);
  Wire.setSCL(17);
  Wire.begin();
  sensor.begin();

  // Find values when the sensor is resting horizontally
  Serial.println("Calculating offsets, do not move MPU6050");
  delay(1000);
  sensor.calcOffsets(true,true);
  Serial.println("Done!\n");
}

// Main Loop
void loop() {
  // call update frequently to get angles
  sensor.update();
  if (millis() > nextReading) {
    Serial.print ("Accel X:");
    Serial.print (sensor.getAccX(), 1);
    Serial.print ("g Y:");
    Serial.print (sensor.getAccY(), 1);
    Serial.print ("g Z:");
    Serial.print (sensor.getAccZ(), 1);
    Serial.println ("g");

    Serial.print ("Gyro X:");
    Serial.print (sensor.getGyroX(), 1);
    Serial.print ("dg/s Y:");
    Serial.print (sensor.getGyroY(), 1);
    Serial.print ("dg/s Z:");
    Serial.print (sensor.getGyroZ(), 1);
    Serial.println ("dg/s");

    Serial.print ("Temp:");
    Serial.print (sensor.getTemp(), 1);
    Serial.println ("C");

    Serial.println ();

    nextReading = millis() + 2000;
  }
}
