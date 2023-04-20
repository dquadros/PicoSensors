// Flame Sensor Example

#define BUZZER_PIN 15
#define SENSOR_PIN 16

// Initializations
void setup() {
  pinMode (BUZZER_PIN, OUTPUT);
  digitalWrite (BUZZER_PIN, LOW);
  pinMode (SENSOR_PIN, INPUT);
}

// Main Loop
void loop() {
  if (digitalRead(SENSOR_PIN) == LOW) {
    digitalWrite (BUZZER_PIN, HIGH);
    delay(500);
    digitalWrite (BUZZER_PIN, LOW);
  }
}
