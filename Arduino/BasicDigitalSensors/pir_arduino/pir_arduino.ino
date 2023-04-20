// Presence (PIR) Sensor Example

#define SENSOR_PIN 16

// Global variables
int counter = 0;

// Initializations
void setup() {
  pinMode (SENSOR_PIN, INPUT);
}

// Main Loop
void loop() {
  Serial.print ("Sensor Triggered ");
  Serial.print (counter);
  Serial.println (" times");
  while (digitalRead(SENSOR_PIN) == LOW)
    ;
  while (digitalRead(SENSOR_PIN) == HIGH)
    ;
  counter++;
}
