# Sound Sensor Example

from machine import Pin
import time

sensor = Pin(16, Pin.IN)
led = Pin(25, Pin.OUT)
led.off()
while True:
    if sensor.value() == 1:
        led.on()
        time.sleep(0.5)
        led.off()
    