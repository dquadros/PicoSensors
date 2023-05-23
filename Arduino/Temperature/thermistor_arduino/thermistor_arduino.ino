// Thermistor Example

#include <math.h>

// Thermistor connection
const int pinTermistor = A0;  

// Circuit Parameters
const float Vcc = 3.3;  
const float R = 2200.0;  

// Thermistor parameters  
float beta;  
float rx;  
float rt1, t1, rt2, t2;
  
// Returns the thermistor resistance
// uses a mean of 20 ADC readings
float getResistance() {
  uint32_t sum = 0;
  for (int i = 0; i < 20; i++) {
    sum += analogRead(pinTermistor);
  }
  float v = (Vcc*sum)/(20*1024.0);
  return R*(Vcc-v)/v;
}

// Gets a reference temperature
void getReference(float &t, float &r) {
  Serial.print("Temperature: ");
  // Wait for a response
  while (Serial.available() == 0) {
    delay(100);
  }
  t = Serial.parseFloat();
  Serial.print(t, 1);
  t += 273.0;
  r = getResistance();
  Serial.print(" Resistance: ");
  Serial.println(r, 0);
}

// Initialization  
void setup() {  
  Serial.begin(115200);
  Serial.setTimeout(10000);
  // Wait for user to press Enter
  while (Serial.read() == -1) {
    delay(100);
  }

  // Get references
  Serial.println("Reference 1");
  getReference (t1, rt1);
  Serial.println("Reference 2");
  getReference (t2, rt2);

  // Compute beta and rx
  beta = log(rt1/rt2)/((1/t1)-(1/t2));
  Serial.print("Beta = ");
  Serial.println(beta, 2);
  rx = rt1 * exp(-beta/t1);
}  
  
// Main Loop
void loop() {  
  float rt = getResistance();
  float t = beta / log(rt/rx);
  Serial.print("Temperature: ");
  Serial.println(t-273.0, 1);
  delay (1000);  
} 