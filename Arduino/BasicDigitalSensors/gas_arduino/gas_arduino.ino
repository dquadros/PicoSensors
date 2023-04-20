// Gas Sensor Example

#define LED_PIN 25
#define SENSOR_PIN 16

// Initialization
void setup() {
  // Init LED gpio
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Init sensor gpio
  pinMode (SENSOR_PIN, INPUT);
}

// Main loop
void loop() {
  if (digitalRead(SENSOR_PIN) == HIGH) {
    digitalWrite(LED_PIN, HIGH);
    delay (500);
    digitalWrite(LED_PIN, LOW);
  }
}
