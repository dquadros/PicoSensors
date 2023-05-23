# Joystick example

import analogio
import pwmio
import board

# Potentiometers
horiz = analogio.AnalogIn(board.A0)
vert = analogio.AnalogIn(board.A1)

# Indicator LEDs, PWM freq is 1KHz
ledRight = pwmio.PWMOut(board.GP12, frequency=1000, duty_cycle=0)
ledUp = pwmio.PWMOut(board.GP13, frequency=1000, duty_cycle=0)
ledDown = pwmio.PWMOut(board.GP14, frequency=1000, duty_cycle=0)
ledLeft = pwmio.PWMOut(board.GP15, frequency=1000, duty_cycle=0)

# Ignore variations up to
DEAD_ZONE = 3000

# Main loop
while True:
    # sample potentiometers
    h = horiz.value
    v = vert.value
    # horizontal axis
    if h > (32767+DEAD_ZONE):
        ledRight.duty_cycle = 0
        ledLeft.duty_cycle = h-32768
    elif h <= (32767-DEAD_ZONE):
        ledRight.duty_cycle = 32767-h
        ledLeft.duty_cycle = 0
    else:
        ledRight.duty_cycle = 0
        ledLeft.duty_cycle = 0

    # vertical axis
    if v > (32767+DEAD_ZONE):
        ledDown.duty_cycle = v-32768
        ledUp.duty_cycle = 0
    elif v <= (32767-DEAD_ZONE):
        ledDown.duty_cycle = 0
        ledUp.duty_cycle = 32767-v
    else:
        ledDown.duty_cycle = 0
        ledUp.duty_cycle = 0

