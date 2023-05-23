// Potentiometer Example

// Pins
#define PIN_SENSOR A0
#define PIN_LED    25

// Initialization
void setup() {
  Serial.begin(115200);
}

// Main loop
void loop() {
  int val = analogRead(PIN_SENSOR);
  Serial.println(val);
// Convert val from 0-1023 to 0-256
  analogWrite(PIN_LED, val>>2);
  delay(200);
}
