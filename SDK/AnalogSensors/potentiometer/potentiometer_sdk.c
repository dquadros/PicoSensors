/**
 * @file potentiometer_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Potentiometer Example
 * @version 1.0
 * @date 2023-05-15
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "hardware/clocks.h"

// Pins
#define LED_PIN 25
#define SENSOR_PIN 27

// PWM configuration
#define WRAP 4095
#define FREQ 1000.0f


// Main Program
int main() {
    // Init stdio
    stdio_init_all();
    #ifdef LIB_PICO_STDIO_USB
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    #endif

    // Init LED gpio
    uint slice_num = pwm_gpio_to_slice_num(LED_PIN);
    uint chan_num = pwm_gpio_to_channel(LED_PIN);
    // Configure the slice
    // f = fsys / (clock divisor * (wrap value+1)
    // clock divisor = fsys / (f * (wrap value+1))
    float fsys = frequency_count_khz(CLOCKS_FC0_SRC_VALUE_CLK_SYS)*1000.0f;
    float div = fsys/(FREQ * (WRAP+1));
    printf("fsys= %.2f div=%.2f\n", fsys, div);
    printf("slice= %u channel=%d\n", slice_num, chan_num);
    pwm_config config = pwm_get_default_config ();
    pwm_config_set_wrap(&config, WRAP);
    pwm_config_set_clkdiv(&config, div);
    pwm_config_set_phase_correct(&config, false);
    pwm_config_set_clkdiv_mode(&config, PWM_DIV_FREE_RUNNING);
    pwm_init(slice_num, &config, false);
    pwm_set_chan_level (slice_num, chan_num, 0);
    pwm_set_enabled(slice_num, true);
    gpio_set_function(LED_PIN, GPIO_FUNC_PWM);

    // Init sensor ADC
    adc_init();
    adc_gpio_init(SENSOR_PIN);
    adc_select_input(0);

    // Main loop
    while(1) {
      uint16_t val = adc_read(); // 0-4095
      printf("%u\n", val);
      pwm_set_chan_level (slice_num, chan_num, val);
      sleep_ms(200);
    }
}
