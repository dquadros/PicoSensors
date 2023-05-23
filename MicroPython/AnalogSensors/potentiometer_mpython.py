# Potentiometer example

from machine import Pin, ADC, PWM
from time import sleep

# Center of potenciometer at ADC0
sensor = ADC(0)

# Internal LED, PWM freq is 1KHz
led = PWM(Pin(25, Pin.OUT))
led.freq(1000)
led.duty_u16(0)
              
# Main loop
while True:
    # read potentiomenter pos
    val = sensor.read_u16()
    print(val)
    # set LED intensity
    led.duty_u16(val)
    # sleep between readings
    sleep(0.2)
