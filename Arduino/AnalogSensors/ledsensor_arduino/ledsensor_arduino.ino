// LED as ligh sensor example

// Pins
#define SENSOR_PIN A0

// initialization
void setup() {
  Serial.begin(115200);
}

// Main loop
void loop() {
  uint32_t sum = 0;
  for (int i = 0; i < 20; i++) {
    sum += analogRead(SENSOR_PIN);
  }
  Serial.println(sum/20);
  delay(2000);
}
