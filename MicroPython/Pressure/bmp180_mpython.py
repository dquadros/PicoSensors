# BMP085 / BMP180 Example
from bmp180 import BMP180
from machine import I2C, Pin
from time import sleep

i2c = I2C(0, sda=Pin(16), scl=Pin(17))
bmp180 = BMP180(i2c)
bmp180.oversample_sett = 2
bmp180.baseline = 101325

print ('ID = {}'.format(hex(bmp180.chip_id[0])))

while True:
    temp = bmp180.temperature
    p = bmp180.pressure
    altitude = bmp180.altitude
    print('Temperature {:.1f}C  Pressure {:.0f}Pa  Altitude {:.2f}m'.format(
        temp, p, altitude))
    sleep(2)
