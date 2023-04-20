# Flame Sensor Example

import digitalio
import board
import time

sensor = digitalio.DigitalInOut(board.GP16)
buzzer = digitalio.DigitalInOut(board.GP15)
buzzer.direction = digitalio.Direction.OUTPUT
buzzer.value = False

while True:
    if sensor.value == 0:
        buzzer.value = True
        time.sleep(0.5)
        buzzer.value = False
