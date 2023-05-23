// Hall Effect Alnalog Sensor

// Pins
#define SENSOR_PIN A0

// Reading for no magnetic field
int zero;

// Read sensor (average 50 readings)
#define N_READINGS 50
uint readSensor() {
  uint32_t sum = 0;
  for (int i = 0; i < N_READINGS; i++) {
    sum += analogRead(SENSOR_PIN);
  }
  return sum / N_READINGS;
}

// initialization
void setup() {
  Serial.begin(115200);

  // Assume no magnetic field at the start
  zero = readSensor() ;
  Serial.print("Zero = ");
  Serial.println(zero);
  delay(1000);
  Serial.println ("Ready");
}

// Main loop
void loop() {
  int field = readSensor();
  if (((field-zero) < 10) && ((field-zero) > -10)) {
      Serial.println ("No field");
  } else if (field > zero) {
    Serial.print ("Field = S ");
    Serial.println(field-zero);
  } else {
    Serial.print ("Field = N ");
    Serial.println(zero-field);
  }
  delay(1000);
}
