# HC-SR04 Ultrasonic Sensor Example

import digitalio
import pulseio
import board
from time import sleep
from time import monotonic

# Set up trigger pin
trigger = digitalio.DigitalInOut(board.GP17)
trigger.direction = digitalio.Direction.OUTPUT
trigger.value = False

# Set up echo pin
echo = pulseio.PulseIn(board.GP16, idle_state=False)
echo.pause()

# Do measurements
while True:
    # restart pulsemeasurement
    echo.clear()
    echo.resume()
    
    # pulse the trigger
    trigger.value = True
    sleep(0.001)
    trigger.value = False
    
    # wait for echo pulse capture
    tout = monotonic() + 0.1
    while (len(echo) == 0) and (monotonic() < tout):
        pass
    echo.pause()

    # Calculate distance
    if len(echo) > 0:
        distance = (echo[0] * 0.0343) / 2
        print('Distance = {0:.1f} cm'.format(distance))
    else:
        print('Timeout')

    sleep(2)
