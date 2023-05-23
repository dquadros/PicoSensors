// Phototransistor example

// Pins
#define SENSOR_PIN A0

// initialization
void setup() {
  Serial.begin(115200);
}

// Main loop
void loop() {
  static uint16_t val_ant = 0;
  uint16_t val = analogRead(SENSOR_PIN);
  int dif = (val > val_ant) ? val - val_ant : val_ant - val;
  if (dif > 100) {
    Serial.println(val);
    val_ant = val;
  }
}
