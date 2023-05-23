# LDR example

from machine import Pin, ADC
from time import sleep
import random

# LDR at ADC0
sensor = ADC(0)

# Buzzer at pin 15
buzzer = Pin(15, Pin.OUT)
buzzer.off()


# Ligh/Dark threshold
DARK = 5000

# Main loop
while True:
    # sleep from 3 to 60 seconds
    sleep(random.uniform(3, 60))
    # read LDR and check if its is dark
    val = sensor.read_u16()
    if val < DARK:
        # short beep
        buzzer.on()
        sleep(0.1)
        buzzer.off()
