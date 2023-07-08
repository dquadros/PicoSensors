# ADXL345 Example
import board
from digitalio import DigitalInOut, Pull
from busio import I2C
from time import sleep
import adafruit_adxl34x

# Set up sensor
i2c = I2C(sda=board.GP16, scl=board.GP17)
sensor = adafruit_adxl34x.ADXL345(i2c)
sensor.enable_motion_detection()
sensor.enable_tap_detection(threshold = 100)
sensor.enable_freefall_detection()

# Main loop: show events
while True:
    sleep(0.1)
    if sensor.events['tap']:
        print('TAP', sensor.acceleration)
    if sensor.events['motion']:
        print('MOTION', sensor.acceleration)
    if sensor.events['freefall']:
        print('FREE FALL', sensor.acceleration)
