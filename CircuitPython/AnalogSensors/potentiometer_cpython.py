# Potentiometer example

import analogio
import pwmio
import board
from time import sleep

# Center of potenciometer at ADC0
sensor = analogio.AnalogIn(board.A0)

# Internal LED, PWM freq is 1KHz
led = pwmio.PWMOut(board.LED, frequency=1000, duty_cycle=0)
              
# Main loop
while True:
    # read potentiomenter pos
    val = sensor.value
    print(val)
    # set LED intensity
    led.duty_cycle=val
    # sleep between readings
    sleep(0.2)
