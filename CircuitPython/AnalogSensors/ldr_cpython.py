# LDR example

import analogio
import digitalio
import board
from time import sleep
import random

# LDR at ADC0
sensor = analogio.AnalogIn(board.A0)

# Buzzer at pin 15
buzzer = digitalio.DigitalInOut(board.GP15)
buzzer.direction = digitalio.Direction.OUTPUT
buzzer.value = False

# Ligh/Dark threshold
DARK = 5000

# Main loop
while True:
    # sleep from 3 to 60 seconds
    sleep(random.uniform(3, 60))
    # read LDR and check if its is dark
    val = sensor.value
    if val < DARK:
        # short beep
        buzzer.value = True
        sleep(0.1)
        buzzer.value = False
