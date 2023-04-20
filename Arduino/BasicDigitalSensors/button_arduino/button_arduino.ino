// Button Example

class Button {
  private:
    int pinButton;
    bool pressed;
    int debounce;
    bool last;
    uint32_t lastTime;

  public:

    // Constructor
    Button (int pin, int debounce = 20) {
      pinButton = pin;
      pinMode (pinButton, INPUT_PULLUP);
      pressed = false;
      this->debounce = debounce;
      last = digitalRead(pinButton) == LOW;
      lastTime = millis();
    }

  // Teste if Button is pressed
  bool isPressed() {
    bool val = digitalRead(pinButton) == LOW;
    if (val != last) {
      // reading changed
      last = val;
      lastTime = millis();
    } else if (val != pressed) {
      int dif = millis() - lastTime;
      if (dif > debounce) {
        // update button status
        pressed = val;
      }
    }
    return pressed;
  }
};

// Global variables
Button button(15);
int counter = 0;

// Initializations
void setup() {
  Serial.begin(115200);
}

// Main loop
void loop() {
  Serial.print ("Button pressed ");
  Serial.print (counter);
  Serial.println (" times");
  while (button.isPressed())
    ;
  while (!button.isPressed())
    ;
  counter++;
}
