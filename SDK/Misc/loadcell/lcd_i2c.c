/**
 * @file lcd_i2c.c
 * @author Daniel Quadros
 * @brief Simple I2C (PCF8574) Alphanumeric LCD Driver
 * @version 0.1
 * @date 2023-07-27
 * 
 * @copyright Copyright (c) 2023
 * 
 */

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#include "lcd_i2c.h"

// I2C Configuration
#define BAUD_RATE 100000   // standard 100KHz

// Connections of the PCF8574 to the display
#define mskRS (1 << 0)
#define mskRW (1 << 1)
#define mskE  (1 << 2)
#define mskBL (1 << 3)
#define mskD4 (1 << 4)
#define mskD5 (1 << 5)
#define mskD6 (1 << 6)
#define mskD7 (1 << 7)

// LCD Control
#define LOW     0
#define HIGH    1
#define CMD             LOW
#define DATA            HIGH
#define CMD_CLS         0x01
#define CMD_DISPON      0x0C
#define CMD_POSCUR      0x80
#define CMD_FUNCTIONSET 0x20
#define LCD_4BITMODE    0x00
#define LCD_2LINE       0x08
#define LCD_5x8DOTS     0x00

// Our state
static i2c_inst_t *i2c;
static uint8_t addr;
static uint8_t curValue;

// Write the current value to the PCF8574
static void write8574() {
    i2c_write_blocking (i2c, addr, &curValue, 1, false);
}

// Control display pin
static void setPin(uint8_t mask, uint8_t value) {
    if (value == LOW) {
        curValue = curValue & ~mask;
    } else {
        curValue = curValue | mask;
    }
    write8574();
}

// Set data pins
static void setData(uint8_t nib) {
    curValue &= ~(mskD4 | mskD5 | mskD6 | mskD7);
    if (nib & 8) {
        curValue |= mskD7;
    }
    if (nib & 4) {
        curValue |= mskD6;
    }
    if (nib & 2) {
        curValue |= mskD5;
    }
    if (nib & 1) {
        curValue |= mskD4;
    }
    write8574();
}

// Send a byte to the display
static void writeByte(uint8_t rs, uint8_t data) {
    setPin(mskRS, rs);
    setPin(mskE, HIGH);
    setData (data >> 4);
    setPin(mskE, LOW);
    setPin(mskE, HIGH);
    setData (data & 0x0F);
    setPin(mskE, LOW);
}

// Init display
void lcd_init (i2c_inst_t *i2cId, int pinSDA, int pinSCL, uint8_t addrPCF) {
    // Save configuration
    i2c = i2cId;
    addr = addrPCF;

    // Set up the I2C
    i2c_init (i2c, BAUD_RATE);
    gpio_set_function(pinSCL, GPIO_FUNC_I2C);
    gpio_set_function(pinSDA, GPIO_FUNC_I2C);
    gpio_pull_up(pinSCL);
    gpio_pull_up(pinSDA);

    // Init PCF8574
    curValue = 0;
    write8574();
    sleep_ms(100);
    setPin(mskRW, LOW);
    writeByte(CMD, 0x03);
    sleep_ms(5);
    writeByte(CMD, 0x03);
    sleep_ms(1);
    writeByte(CMD, 0x03);
    sleep_ms(1);
    writeByte(CMD, 0x02);
    sleep_ms(1);
    writeByte(CMD, CMD_FUNCTIONSET | LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS);
    sleep_ms(1);
    writeByte(CMD, CMD_CLS);
    sleep_ms(2);
    writeByte(CMD, CMD_DISPON);
}

// Control backlight
void lcd_backlight(bool on) {
    setPin(mskBL, on ? HIGH : LOW);
}

// Clear display
void lcd_clear() {
    writeByte(CMD, CMD_CLS);
    sleep_ms(2);
}

// Write text
void lcd_write(uint8_t lin, int8_t col, const char *text) {
    uint8_t ender = col;
    if (lin == 1) {
        ender += 0x40;
    }
    writeByte(CMD, CMD_POSCUR + ender);
    while (*text) {
        writeByte(DATA, (uint8_t) *text++);
    }
}
