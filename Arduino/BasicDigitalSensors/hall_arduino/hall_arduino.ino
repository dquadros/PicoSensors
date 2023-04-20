// Hall Efect Sensor Example

// Pins used
#define LED_PIN    25
#define SENSOR_PIN 16
#define STEPPER_1  2
#define STEPPER_2  3
#define STEPPER_3  4
#define STEPPER_4  5

// what pins to turn on at each step
int steps[4][4] = {
  { HIGH, LOW,  LOW,  HIGH },
  { HIGH, HIGH, LOW,  LOW  },
  { LOW,  HIGH, HIGH, LOW  },
  { LOW,  LOW,  HIGH, HIGH }
};

// Stepper motor control class
class Stepper {
  private:
    int pins[4];
    int step;

    void setpins (const int *values) {
      for (int i = 0; i < 4; i++) {
        digitalWrite (pins[i], values[i]);
      }
    }

  public:
    // Constructor
    Stepper (int pin1, int pin2, int pin3, int pin4) {
      pins[0] = pin1;
      pins[1] = pin2;
      pins[2] = pin3;
      pins[3] = pin4;
      step = 0;
      for (int i = 0; i < 4; i++) {
        pinMode (pins[i], OUTPUT);
        digitalWrite(pins[i], LOW);
      }
    }

    // Advance one step
    void onestep() {
      setpins (steps[step]);
      if (++step == 4) {
        step = 0;
      }
    }
};

// Hall Effect Sensor
class HallSensor {
  private:
    int pin;
    bool state;
    uint32_t last, elapsed;
  public:
    // constructor
    HallSensor(int pinSensor) {
      pin = pinSensor;
      pinMode (pin, INPUT_PULLUP);
      state = digitalRead(pin);
      last = elapsed = 0;
    }

    // detect sensor
    bool detect() {
      bool read = digitalRead(pin) == HIGH;
      if (read != state) {
        state = read;
        uint32_t aux = millis();
        if (read) {
          if (last != 0) {
            elapsed = aux - last;
          }
          last = aux;
          return true;
        }
      }
      return false;
    }

    // return time since previous detection
    uint32_t getElapsed() {
      return elapsed;
    }
};

Stepper stepper(STEPPER_1, STEPPER_2, STEPPER_3, STEPPER_4);
HallSensor sensor(SENSOR_PIN);

void setup() {
  Serial.begin(115200);
  Serial.println("Hall Effect Sensor Example");

  // Init LED gpio
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

void loop() {
    // choose a random speed
    int delay = (rand() % 1700) + 1500;
    Serial.print ("Delay = ");
    Serial.print (delay/1000.0);
    Serial.println (" ms");
    uint32_t changeTime = millis() + 30000;
    while (millis() < changeTime) {
      stepper.onestep();
      if (sensor.detect()) {
        digitalWrite(LED_PIN, HIGH);
        uint32_t elapsed = sensor.getElapsed();
        if (elapsed != 0) {
          Serial.print ("RPM = ");
          Serial.println (60000.0 / elapsed);
        }
        sleep_us(delay);
        digitalWrite (LED_PIN, LOW);
      }
      else {
        sleep_us(delay);
      }
    }
}
