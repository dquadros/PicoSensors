// QMC5883L & HMC5883L Example

#include <DFRobot_QMC5883.h>

//DFRobot_QMC5883 compass(&Wire, QMC5883_ADDRESS);
DFRobot_QMC5883 compass(&Wire, HMC5883L_ADDRESS);


// Initialization
void setup() {
  while(!Serial) {
    delay(100);
  }
  Serial.begin(115200);
  Wire.setSDA(16);
  Wire.setSCL(17);
  while (!compass.begin())
  {
    Serial.println("Could not find a valid 5883 sensor, check wiring!");
    delay(2000);
  }
  Serial.println(compass.isHMC() ? "HMC5883L" : "QMC5883L");
}

// Main Loop
void loop() {
  // Reads the data from the sensor and returns a reference to
  // the internal vector
  sVector_t mag = compass.readRaw();
  // Fills the heading field in the vector
  compass.getHeadingDegrees();
  Serial.print("Heading: ");
  Serial.println(mag.HeadingDegress,1);
  delay(2000);
}
