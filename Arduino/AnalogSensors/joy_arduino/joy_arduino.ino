// Analog Joystick Example

// LED connections
#define LED_RIGHT 12
#define LED_UP    13
#define LED_DOWN  14
#define LED_LEFT  15

// Joystick Connections
#define PIN_HORIZ   A0
#define PIN_VERTIC  A1

// Ignore small variations around the center
#define DEAD_ZONE 30

// Initialization
void setup() {
}

// Main loop
void loop() {
  // Read the joystick position
  int horiz = analogRead (PIN_HORIZ);
  int vertic = analogRead (PIN_VERTIC);

// Light the LEDs according to the position
  if (horiz > (511 + DEAD_ZONE)) {
    analogWrite(LED_RIGHT, 0);
    analogWrite(LED_LEFT, (horiz-512) >> 1);
  } else if (horiz <= (511 - DEAD_ZONE)){
    analogWrite(LED_RIGHT, (511-horiz) >> 1);
    analogWrite(LED_LEFT, 0);
  } else {
    analogWrite(LED_RIGHT, 0);
    analogWrite(LED_LEFT, 0);
  }
  
  if (vertic > (511 + DEAD_ZONE)) {
    analogWrite(LED_DOWN, (vertic-512) >> 1);
    analogWrite(LED_UP, 0);
  } else if (vertic <= (511 - DEAD_ZONE)){
    analogWrite(LED_DOWN, 0);
    analogWrite(LED_UP, (511-vertic) >> 1);
  } else {
    analogWrite(LED_UP, 0);
    analogWrite(LED_DOWN, 0);
  }
}
