// LM35D and TMP36 Temperature Sensor Example

// Pins (-1 if not used)
#define LM35_PIN A0
#define TMP36_PIN A1

//returns the voltage in an ADC pin 
// averages 10 readings
#define N_READINGS 10
float readSensor(int pin) {
  uint32_t sum = 0;

  if (pin == -1) {
    return 0;
  }

  for (int i = 0; i < N_READINGS; i++) {
    sum += analogRead(pin);
  }
  return (sum*3.3 / N_READINGS)/1024.0;
}

// initialization
void setup() {
  Serial.begin(115200);
}

// Main loop
void loop() {
  float tempLM35 = readSensor(LM35_PIN)/0.01;
  float tempTMP36 = 25.0 + (readSensor(TMP36_PIN)-0.75)/0.01;
  Serial.print("LM35 = ");
  Serial.print(tempLM35,1);
  Serial.print("C  TMP36 = ");
  Serial.print(tempTMP36,1);
  Serial.println("C");
  delay(2000);
}