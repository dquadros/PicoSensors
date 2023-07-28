/**
 * @file hcsr04_sdk.c
 * @author Daniel Quadros
 * @brief HC-SR04 Ultrasonic Sensor Example
 * @version 1.0
 * @date 2023-07-24
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include "stdio.h"
#include "string.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/i2c.h"
#include "lcd_i2c.h"

// Our PIO program:
#include "hx711.pio.h"

// Sensor connections
#define PIN_DATA    12
#define PIN_CLOCK   13

// Display connections
#define PIN_SDA 16
#define PIN_SCL 17

// Button connections
#define PIN_BUTTON 15

// PIO
static PIO pio = pio0;
static uint offset;
static uint sm;

// Parameters to convert value to weigh in kg
float tare, scale;

// Local routines
void keyPress(const char *msg);
void calibrate(void);
float hx711_avg(int count);

// Main Program
int main() {
    // Init and start the state machine
    sm = pio_claim_unused_sm(pio, true);
    offset = pio_add_program(pio, &hx711_program);
    hx711_program_init(pio, sm, offset, PIN_DATA, PIN_CLOCK);

    // Init display
    lcd_init (i2c0, PIN_SDA, PIN_SCL, 0x27);
    lcd_backlight(true);

    // Init button
    gpio_init(PIN_BUTTON);
    gpio_set_pulls(PIN_BUTTON, true, false);

    // Calibatre the sensor
    calibrate();
    lcd_write(0, 0, "Scale Ready");

    // Main Loop
    while (true) {
        char buf[20];
        float weight = (hx711_avg(10) - tare)/scale;
        sprintf(buf,"%7.3f", weight);
        strcat(buf,"kg");
        lcd_write(1, 0, buf);
        sleep_ms(500);
    }
}

// Get an average reading
float hx711_avg(int count) {
    int32_t sum = 0;
    pio_sm_clear_fifos (pio, sm);
    for (int i = 0; i < count; i++) {
        int32_t val = pio_sm_get_blocking(pio, sm);
        if (val & 0x800000) {
            val = val - 0x1000000;
        }
        sum += val;
    }
    return ((float)sum)/count;
}

// Initial calibration
void calibrate() {
    keyPress("Empty scale");
    tare = hx711_avg(50);
    keyPress("Put 1kg");
    scale = (hx711_avg(50) - tare);
}

// Wait for a key press
void keyPress(const char *msg) {
    lcd_clear();
    lcd_write(0,0,msg);
    lcd_write(1,0,"Press button");
    while (gpio_get(PIN_BUTTON) == 1) {
        sleep_ms(100);
    }
    sleep_ms(100); // debounce
    lcd_write(1,0,"Release button");
    while (gpio_get(PIN_BUTTON) == 0) {
        sleep_ms(100);
    }
    sleep_ms(100); // debounce
    lcd_write(1,0,"Wait          ");
}
