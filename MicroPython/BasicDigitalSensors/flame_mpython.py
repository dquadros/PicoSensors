# Flame Sensor Example

from machine import Pin
import time

sensor = Pin(16, Pin.IN)
buzzer = Pin(15, Pin.OUT)
buzzer.off()
while True:
    if sensor.value() == 0:
        buzzer.on()
        time.sleep(0.5)
        buzzer.off()
