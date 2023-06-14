# BMP390 Sensor Example
import board
from busio import I2C
from time import sleep
from adafruit_bmp3xx import BMP3XX_I2C

i2c = I2C(sda=board.GP16, scl=board.GP17)
bmp = BMP3XX_I2C(i2c)

while True:
    press = bmp.pressure
    temp = bmp.temperature
    alt = bmp.altitude
    print ('Temperature: {:.1f}C  Pressure: {:.0f}Pa  Altitude: {:.1f}m'.format(
        temp, press*100, alt))
    sleep(2)
