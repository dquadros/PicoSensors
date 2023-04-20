/**
 * @file button_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Button Example
 * @version 1.0
 * @date 2023-04-18
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"


static inline uint32_t board_millis(void)
{
	return to_ms_since_boot(get_absolute_time());
}

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
      gpio_init(pinButton);
      gpio_set_pulls(pinButton, true, false);
      pressed = false;
      this->debounce = debounce;
      last = gpio_get(pinButton) == 0;
      lastTime = board_millis();
    }

  // Teste if Button is pressed
  bool isPressed() {
    bool val = gpio_get(pinButton) == 0;
    if (val != last) {
      // reading changed
      last = val;
      lastTime = board_millis();
    } else if (val != pressed) {
      int dif = board_millis() - lastTime;
      if (dif > debounce) {
        // update button status
        pressed = val;
      }
    }
    return pressed;
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

    printf("\nButton Example\n");

    Button button(15);
    int counter = 0;

    while(1) {
        printf ("Button pressed %d times\n", counter);
        while (button.isPressed())
            ;
        while (!button.isPressed())
            ;
        counter++;
    }
}