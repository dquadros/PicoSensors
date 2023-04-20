# Presence (PIR) Sensor Example

from machine import Pin
import time

sensor = Pin(16, Pin.IN)
counter = 0
while True:
    print ("Sensor triggered {} times".format(counter))
    while sensor.value() == 0:
        time.sleep_ms(1)
    while sensor.value() == 1:
        time.sleep_ms(1)
    counter = counter+1
