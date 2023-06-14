// BMP390 Sensor Example
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BMP3XX.h"

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BMP3XX bmp;

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
	if (!bmp.begin_I2C()) {
		Serial.println("Sensor not found.");
		while (true)
      ;
	}

  // Set up oversampling and filter initialization
  bmp.setTemperatureOversampling(BMP3_OVERSAMPLING_8X);
  bmp.setPressureOversampling(BMP3_OVERSAMPLING_4X);
  bmp.setIIRFilterCoeff(BMP3_IIR_FILTER_COEFF_3);
  bmp.setOutputDataRate(BMP3_ODR_50_HZ);
}

// Main Loop
void loop() {
	delay(2000);
	if (bmp.performReading()) {
    Serial.print("Temperature: ");
    Serial.print(bmp.temperature,2);
    Serial.print("C  Pressure: ");
    Serial.print(bmp.pressure,0);
    Serial.print("Pa  Altitude: "); 
    Serial.print(bmp.readAltitude(SEALEVELPRESSURE_HPA));
    Serial.println("m");
  }
}
