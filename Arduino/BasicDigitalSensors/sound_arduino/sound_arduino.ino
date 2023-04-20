// Sound Sensor Example

#define SENSOR_PIN 16

// Initializations
void setup() {
  pinMode (LED_BUILTIN, OUTPUT);
  digitalWrite (LED_BUILTIN, LOW);
  pinMode (SENSOR_PIN, INPUT);
}

// Main Loop
void loop() {
  if (digitalRead(SENSOR_PIN) == HIGH) {
    digitalWrite (LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite (LED_BUILTIN, LOW);
  }
}
