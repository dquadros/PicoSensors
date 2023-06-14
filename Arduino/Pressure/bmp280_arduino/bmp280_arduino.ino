// BMP280/BME280 Example
#include <Arduino.h>
#include <Wire.h>
#include <BMx280I2C.h>

#define I2C_ADDRESS 0x76

BMx280I2C bmx280(I2C_ADDRESS);

// Initialization
void setup() {
  // Init serial
  Serial.begin (115200);
	while (!Serial)
    ;

  // Init I2C
  Wire.setSDA(16);
  Wire.setSCL(17);
	Wire.begin();

  // Init sensor
	if (!bmx280.begin()) {
		Serial.println("Sensor not found.");
		while (true)
      ;
	}

  // Identify sensor
  Serial.println (bmx280.isBME280()? "BME280 Sensor" : "BMP280 Sensor");

	//reset sensor to default parameters.
	bmx280.resetToDefaults();
	bmx280.writeOversamplingPressure(BMx280MI::OSRS_P_x16);
	bmx280.writeOversamplingTemperature(BMx280MI::OSRS_T_x16);
	if (bmx280.isBME280()) {
		bmx280.writeOversamplingHumidity(BMx280MI::OSRS_H_x16);
  }
}

// Main Loop
void loop() {
	delay(2000);
	if (bmx280.measure()) {
    //wait for the measurement to finish
    do {
      delay(10);
    } while (!bmx280.hasValue());

    Serial.print("Temperature: ");
    Serial.print(bmx280.getTemperature(),2);
    Serial.print("  Pressure: ");
    Serial.print(bmx280.getPressure(),0);
  	if (bmx280.isBME280()) {
		  Serial.print("  Humidity: "); 
		  Serial.print(bmx280.getHumidity());
	  }
    Serial.println();
  }
}
