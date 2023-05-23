// LDR example

// Pins
#define BUZZER_PIN 15
#define SENSOR_PIN A0

// Ligh/Dark threshold
#define DARK 75

// initialization
void setup() {
  pinMode (BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

// Main loop
void loop() {
  // Sleep for 3 to 60 seconds
  //uint tsleep = (rand() % 59000) + 3000;
  uint tsleep = (rand() % 3000) + 1000;
  delay (tsleep);
  // Short beep if dark
  uint16_t val = analogRead(SENSOR_PIN); // 0-1023
  if (val < DARK) {
    digitalWrite (BUZZER_PIN, HIGH);
    delay (100);
    digitalWrite(BUZZER_PIN, LOW);
  }
}
