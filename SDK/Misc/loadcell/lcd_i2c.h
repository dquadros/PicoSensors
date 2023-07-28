// Simple I2C (PCF8574) Alphanumeric LCD Driver

#ifndef LCD_I2C_H

#define LCD_I2C_H

void lcd_init (i2c_inst_t *i2cId, int pinSDA, int pinSCL, uint8_t addrPCF);
void lcd_backlight(bool on);
void lcd_clear(void);
void lcd_write(uint8_t lin, int8_t col, const char *text);

#endif
