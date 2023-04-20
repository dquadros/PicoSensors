# Vibration Sensor Example

import digitalio
import board
import time

sensor = digitalio.DigitalInOut(board.GP16)
led = digitalio.DigitalInOut(board.GP25)
led.direction = digitalio.Direction.OUTPUT
led.value = False
vibr = []
count = 0
while True:
    time.sleep(0.01)
    if len(vibr) >= 100:
        # discard oldest
        count = count - vibr.pop()
    vibr.insert(0, sensor.value)
    count = count + vibr[0]
    led.value = count > 80
