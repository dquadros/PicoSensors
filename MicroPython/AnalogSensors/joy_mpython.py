# Joystick example

from machine import Pin, ADC, PWM
from time import sleep

# Potentiometers
horiz = ADC(0)
vert = ADC(1)

# Indicator LEDs, PWM freq is 1KHz
def initPWM(pin):
    pwm = PWM(Pin(pin, Pin.OUT))
    pwm.freq(1000)
    pwm.duty_u16(0)
    return pwm
    
ledRight = initPWM(12)
ledUp = initPWM(13)
ledDown = initPWM(14)
ledLeft = initPWM(15)

# Ignore variations up to
DEAD_ZONE = 3000

# Main loop
while True:
    # sample potentiometers
    h = horiz.read_u16()
    v = vert.read_u16()
    # horizontal axis
    if h > (32767+DEAD_ZONE):
        ledRight.duty_u16(0)
        ledLeft.duty_u16(h-32768)
    elif h <= (32767-DEAD_ZONE):
        ledRight.duty_u16(32767-h)
        ledLeft.duty_u16(0)
    else:
        ledRight.duty_u16(0)
        ledLeft.duty_u16(0)

    # vertical axis
    if v > (32767+DEAD_ZONE):
        ledDown.duty_u16(v-32768)
        ledUp.duty_u16(0)
    elif v <= (32767-DEAD_ZONE):
        ledDown.duty_u16(0)
        ledUp.duty_u16(32767-v)
    else:
        ledDown.duty_u16(0)
        ledUp.duty_u16(0)

