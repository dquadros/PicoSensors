# Button Example

import digitalio
import board
import time

def time_ms():
    return time.monotonic_ns() // 1000000

class Button:
    def __init__(self, pin, debounce=20):
        self.pinButton = digitalio.DigitalInOut(pin)
        self.pinButton.pull = digitalio.Pull.UP
        self.pressed = False
        self.debounce = debounce
        self.last = self.pinButton.value
        self.lastTime = time_ms()
        
    def isPressed(self):
        val = self.pinButton.value
        if val != self.last:
            # reading changed
            self.last = val
            self.lastTime = time_ms()
        elif (val == 0) != self.pressed:
            dif = time_ms() - self.lastTime
            if dif > self.debounce:
                # update button state
                self.pressed = val == 0
        return self.pressed
        
# Test program
button = Button(board.GP15)
counter = 0
while True:
    print ("Button pressed {} times".format(counter))
    while button.isPressed():
        time.sleep(0.001)
    while not button.isPressed():
        time.sleep(0.001)
    counter = counter+1
