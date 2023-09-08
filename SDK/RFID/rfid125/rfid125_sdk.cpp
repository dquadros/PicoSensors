/**
 * @file rfid125_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief 125kHz RFID Reader Example
 * @version 1.0
 * @date 2023-09-07
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "pico/stdlib.h"
#include "hardware/clocks.h"
#include "hardware/uart.h"
#include "hardware/pwm.h"

// Conexoes
#define UART_ID       uart0
#define UART_RX_PIN   13
#define LED_PIN       25
#define SERVO_PIN     17
#define BUZZER_PIN    2
#define BUTTON_PIN    16

static inline uint32_t board_millis(void)
{
	return to_ms_since_boot(get_absolute_time());
}

//  Class to control a buzzer
class Buzzer {
  private:
    int pinBuzzer;
  public:
    Buzzer(int pin) {
      pinBuzzer = pin;
      gpio_init (pinBuzzer);
      gpio_set_dir (pinBuzzer, true);
      gpio_put (pinBuzzer, false);
    }

    void beep() {
      gpio_put (pinBuzzer, true);
      sleep_ms(300);
      gpio_put (pinBuzzer, false);
    }
};

// Class to check a button
class Button {
  private:
    int pinButton;
    bool pressed;
    int debounce;
    bool last;
    uint32_t lastTime;
 public:
    Button(int pin, int debounce = 20) {
      pinButton = pin;
      gpio_init(pinButton);
      gpio_set_pulls(pinButton, true, false);
      pressed = false;
      this->debounce = debounce;
      last = gpio_get(pinButton) == 0;
      lastTime = board_millis();
    }

  // Test if button pressed and released
  bool released() {
    bool val = gpio_get(pinButton) == 0;
    if (val != last) {
      // reading changed
      last = val;
      lastTime = board_millis();
    } else if (val != pressed) {
      int dif = board_millis() - lastTime;
      if (dif > debounce) {
        // update button state
        pressed = val;
        return !pressed;
      }
    }
    return false;
  }
};

// Class to control a servomotor
// For more datails on PWM programming, please see
// my book "Knowing the RP2040"
class Servo {
  private:
    const float FREQ = 50.0;  // 20 ms period
    const int WRAP = 20000;    // each count is 1 us
    uint slice_num;
    uint channel;
    uint time_0deg, time_180deg;
  public:
    Servo (int pin, int time0=600, int time180=2400) {
      slice_num = pwm_gpio_to_slice_num(pin);
      channel = pwm_gpio_to_channel(pin);
      float fsys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS)*1000.0f;
      float div = fsys/(FREQ * (WRAP+1));
      printf("fsys= %.2f div=%.2f\n", fsys, div);
      pwm_config config = pwm_get_default_config ();
      pwm_config_set_wrap(&config, WRAP);
      pwm_config_set_clkdiv(&config, div);
      pwm_config_set_phase_correct(&config, false);
      pwm_config_set_clkdiv_mode(&config, PWM_DIV_FREE_RUNNING);
      pwm_init(slice_num, &config, false);
      pwm_set_chan_level(slice_num, channel, 0);
      pwm_set_enabled(slice_num, true);
      gpio_set_function(pin, GPIO_FUNC_PWM);
      time_0deg = time0;
      time_180deg = time180;
    }

    void pos(int angle) {
      uint ontime = time_0deg + (time_180deg-time_0deg)*angle/180;
      pwm_set_chan_level(slice_num, channel, ontime);
      sleep_ms(400); // time for the motor to move
      pwm_set_chan_level(slice_num, channel, 0);
    }
};

// Class to get RFID reader messages
class RFID {
  private:
    uart_inst_t *uart;
    uint8_t last[9];
    uint32_t last_read;
    uint8_t bufRx[14];
    int pos;

    inline uint8_t decodHex(uint8_t c) {
      if ((c >= '0') && (c <= '9')) {
        return c - '0';
      }
      if ((c >= 'A') && (c <= 'F')) {
        return c - 'A' + 10;
      }
      return 0;
    }

  public:
    RFID(uart_inst_t *uartid, int pinRx) {
      uart = uartid;
      gpio_set_function(pinRx, GPIO_FUNC_UART);
      uart_init (uart, 9600);
      uart_set_format (uart, 8, 1, UART_PARITY_NONE);
      uart_set_fifo_enabled (uart, true);
      pos = 0;
      last[0] = 0;
      last_read = 0;
    }

    bool read(char *tag) {
      if (uart_is_readable(uart) == 0) {
        uint32_t ellapsed = board_millis() - last_read;
        if ((last[0] != 0) && (ellapsed > 1000)) {
          // Long time without messages, forget last tag
          last[0] = 0;
        }
        return false;
      }
      uint8_t c = uart_getc(uart);
      if ((pos == 0) && (c != 0x02)) {
        return false;
      }
      bufRx[pos++] = c;
      if (pos == 14) {
        pos = 0;
        if (c == 0x03) {
          last_read = board_millis();
          uint8_t crc = 0;
          uint8_t x;
          for (int i = 1; i < 13; i = i+2) {
            x = (decodHex(bufRx[i]) << 4) + decodHex(bufRx[i+1]);
            crc ^= x;
          }
          if (crc == 0) {
            if (memcmp (bufRx+3, last, 8) != 0) {
              memcpy(last, bufRx+3, 8);
              last[8] = 0;
              strcpy(tag, (const char *)last);
              return true;
            }
          }
        }
      }
      return false;
    }

};

// Authorized tags list
#define MAX_TAGS 10
char autorized[MAX_TAGS][9];
int nAuthorized = 0;

// Check if a tag is authorized
bool isAuthorized(char * tag) {
  for (int i = 0; i < nAuthorized; i++) {
    if (strcmp(autorized[i], tag) == 0) {
      return true;
    }
  }
  return false;
}


// Main Program
int main() {
  // Init stdio
  stdio_init_all();
  #ifdef LIB_PICO_STDIO_USB
  while (!stdio_usb_connected()) {
      sleep_ms(100);
  }
  #endif
  printf ("\n125kHz RFID Example\n\n");

  // Init LED
  gpio_init (LED_PIN);
  gpio_set_dir (LED_PIN, true);
  gpio_put (LED_PIN, false);

  // Create objects
  Buzzer buzzer(BUZZER_PIN);
  Button button(BUTTON_PIN);
  Servo servo(SERVO_PIN);
  RFID rfid(UART_ID, UART_RX_PIN);

  // Blink LED, "close door" and beep
  gpio_put (LED_PIN, true);
  buzzer.beep();
  servo.pos(0);
  gpio_put (LED_PIN, false);

  // Main Loop
  bool enroll = false;
  uint32_t closeTime = 0;
  char tag[9];
  while(1) {
    if ((closeTime != 0) && (board_millis() > closeTime)) {
      servo.pos(0);
      closeTime = 0;
    }
    if (button.released()) {
      enroll = true;
    }
    if (rfid.read(tag)) {
      if (enroll) {
        if (isAuthorized(tag)) {
          printf ("Tag %s  already authorized\n", tag);
        } else if (nAuthorized == MAX_TAGS) {
          printf ("Authorized tag list is full\n");
        } else {
          gpio_put (LED_PIN, true);
          strcpy (autorized[nAuthorized++], tag);
          printf ("Tag %s authorized\n", tag);
          sleep_ms(300);
          gpio_put (LED_PIN, false);
        }
        enroll = false;
      }
      else if (isAuthorized(tag)) {
          printf ("Tag %s authorized\n", tag);
          servo.pos(180); // "open door"
          closeTime = board_millis()+3000;
      } else {
          printf ("Tag %s NOT authorized\n", tag);
          buzzer.beep();
      }
    }
  }
}
