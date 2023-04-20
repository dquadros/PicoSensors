/**
 * @file hall_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Hall Effect Sensor Example
 * @version 1.0
 * @date 2023-04-18
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

// Pins used
#define LED_PIN    25
#define SENSOR_PIN 16
#define STEPPER_1  2
#define STEPPER_2  3
#define STEPPER_3  4
#define STEPPER_4  5

static inline uint32_t board_millis(void)
{
	return to_ms_since_boot(get_absolute_time());
}

// what pins to turn on at each step
int steps[4][4] = {
  { 1, 0, 0, 1 },
  { 1, 1, 0, 0 },
  { 0, 1, 1, 0 },
  { 0, 0, 1, 1 }
};

// Stepper motor control class
class Stepper {
  private:
    int pins[4];
    int step;

    void setpins (const int *values) {
      for (int i = 0; i < 4; i++) {
        gpio_put (pins[i], values[i]);
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
        gpio_init (pins[i]);
        gpio_set_dir (pins[i], true);
        gpio_put (pins[i], 0);
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
      gpio_init (pin);
      gpio_set_pulls(pin, true, false);
      state = gpio_get(pin);
      last = elapsed = 0;
    }

    // detect sensor
    bool detect() {
      bool read = gpio_get(pin);
      if (read != state) {
        state = read;
        uint32_t aux = board_millis();
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

// Main Program
int main() {
  // Init stdio
  stdio_init_all();
  #ifdef LIB_PICO_STDIO_USB
  while (!stdio_usb_connected()) {
      sleep_ms(100);
  }
  #endif

  printf("\nHall Effect Example\n");

  Stepper stepper(STEPPER_1, STEPPER_2, STEPPER_3, STEPPER_4);
  HallSensor sensor(SENSOR_PIN);

  // Init LED gpio
  gpio_init (LED_PIN);
  gpio_set_dir (LED_PIN, true);
  gpio_put (LED_PIN, false);

  // Main loop
  while(1) {
    // choose a random speed
    int delay = (rand() % 1700) + 1500;
    printf ("Delay = %.1f ms\n", delay/1000.0);
    uint32_t changeTime = board_millis() + 30000;
    while (board_millis() < changeTime) {
      stepper.onestep();
      if (sensor.detect()) {
        gpio_put (LED_PIN, true);
        uint32_t elapsed = sensor.getElapsed();
        if (elapsed != 0) {
          printf ("RPM = %.1f\n", 60000.0 / elapsed);
        }
        sleep_us(delay);
        gpio_put (LED_PIN, false);
      }
      else {
        sleep_us(delay);
      }
    }
  }
}