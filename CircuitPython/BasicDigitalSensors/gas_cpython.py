# Gas Sensor Example

import digitalio
import board
import time

sensor = digitalio.DigitalInOut(board.GP16)
led = digitalio.DigitalInOut(board.GP25)
led.direction = digitalio.Direction.OUTPUT
led.value = False

while True:
    if sensor.value == 1:
        led.value = True
        time.sleep(0.5)
        led.value = False
