# BME280 Example
from machine import I2C, Pin
from time import sleep
import bme280

i2c = I2C(0, sda=Pin(16), scl=Pin(17))
bme = bme280.BME280(i2c=i2c)

while (True):
    temp, press, humid = bme.read_compensated_data()
    print('Temperature {:.1f}C  Pressure {:.0f}Pa  Humidity {:.1f}%'.format(
        temp/100, press/256, humid/1024))
    sleep(2)
