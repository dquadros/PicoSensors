// LoadCell Example

#include <HX711.h>
#include <Wire.h>
#include <LiquidCrystal_PCF8574.h>

// LCD access
LiquidCrystal_PCF8574 lcd(0x27);

// HX711 ADC
const int HX711_DT = 12;
const int HX711_CK = 13;
HX711 sensor;

// Button
const int BUTTON = 15;

// Initialization
void setup() {
  // Init button
  pinMode (BUTTON, INPUT_PULLUP);

  // Init display
  Wire.setSDA(16);
  Wire.setSCL(17);
  Wire.begin();
  lcd.begin(16, 2);
  lcd.setBacklight(255);

  // Initial callibration
  sensor.begin(HX711_DT, HX711_CK);
  keyPress("Empty scale");
  sensor.tare(50);
  keyPress("Put 1kg");
  long reading = sensor.read_average(50);
  sensor.set_scale ((reading - sensor.get_offset())/1.00f);
  lcd.clear();
  lcd.print("Scale Ready");
}

// Main loop
void loop() {
  // read and show weight
  float weight = sensor.get_units();
  char msg[17];
  dtostrf (weight, 7, 3, msg);
  strcat (msg, "kg");
  lcd.setCursor(0,1);
  lcd.print(msg);
  
  // wait a time between readings
  delay (500);  
}

// Wait for key press
void keyPress(const char *msg) {
  lcd.clear();
  lcd.print(msg);
  lcd.setCursor(0,1);
  lcd.print("Press button");
  while (digitalRead(BUTTON) == HIGH) {
    delay(100);
  }
  delay(100); // debounce
  lcd.setCursor(0,1);
  lcd.print("Release button");
  while (digitalRead(BUTTON) == LOW) {
    delay(100);
  }
  delay(100); // debounce
  lcd.setCursor(0,1);
  lcd.print("Wait          ");
}
