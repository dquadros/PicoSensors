# BME280 Sensor Example
import board
from busio import I2C
from time import sleep
from adafruit_bme280 import basic as bme280

i2c = I2C(sda=board.GP16, scl=board.GP17)
bme = bme280.Adafruit_BME280_I2C(i2c, 0x76)

while True:
    press = bme.pressure
    temp = bme.temperature
    alt = bme.altitude
    humid = bme.humidity
    print ('Temperature: {:.1f}C  Pressure: {:.0f}Pa  Altitude: {:.1f}m  Humidity: {:.1f}%'.format(
        temp, press*100, alt, humid))
    sleep(2)
