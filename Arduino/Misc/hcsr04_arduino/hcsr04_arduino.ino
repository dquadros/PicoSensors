// HC-SR04 Ultrasonic Sensor Example

#define PIN_TRIGGER 17
#define PIN_ECHO    16

// Initialization
void setup() {
  // Init the serial
  while (!Serial) {
    delay(100);
  }
  Serial.begin(115200);

  // Set up pins  
  pinMode (PIN_TRIGGER, OUTPUT);
  pinMode (PIN_ECHO, INPUT);
  digitalWrite(PIN_TRIGGER, LOW);
}

// Main loop
void loop() {
  // Pulse trigger pin
  digitalWrite(PIN_TRIGGER, HIGH);
  delayMicroseconds(10);
  digitalWrite(PIN_TRIGGER, LOW);

  // Measure echo pin
  uint32_t ellapsed = pulseIn(PIN_ECHO, HIGH, 100000);

  // Calculate distance
  if (ellapsed != 0) {
    float distance = (ellapsed * 0.0343) / 2.0;
    Serial.print("Distance = ");
    Serial.print(distance, 1);
    Serial.println(" cm");
    delay(2000);
  } else {
    Serial.println("** TIMEOUT **");
  }
}
