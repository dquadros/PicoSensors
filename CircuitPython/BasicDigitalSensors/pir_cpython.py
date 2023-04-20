# Presence (PIR) Sensor Example

import digitalio
import board
import time

sensor = digitalio.DigitalInOut(board.GP16)
counter = 0
while True:
    print ("Sensor triggered {} times".format(counter))
    while sensor.value == 0:
        time.sleep(0.001)
    while sensor.value == 1:
        time.sleep(0.001)
    counter = counter+1
