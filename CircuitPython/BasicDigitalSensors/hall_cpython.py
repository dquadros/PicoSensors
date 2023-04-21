# Hall Effect Sensor Example

import digitalio
import board
import time
import random

def time_ms():
    return time.monotonic_ns() // 1000000

#   Stepper motor control class
class Stepper():
  # set motor control pins
  def setpins(self, val=[0,0,0,0]):
    for i in range(len(self.pins)):
      self.pins[i].value = val[i]
 
  # init
  def __init__(self, pins=None):
    if pins is None:
      raise ValueError("Must specify pins!")
    if len(pins) != 4:
      raise ValueError("There must be 4 pins")
    self.pins = []
    for pin in pins:
        digPin = digitalio.DigitalInOut(pin)
        digPin.direction = digitalio.Direction.OUTPUT
        self.pins.append(digPin)
    self.steps = [[1,0,0,1], [1,1,0,0], [0,1,1,0], [0,0,1,1]]
    self.setpins()
    self.step = 0
 
  # advance one step
  def onestep(self):
    self.setpins(self.steps[self.step])
    self.step = self.step+1
    if self.step >= len(self.steps):
      self.step = 0

# Hall Effect sensor
class HallSensor:
  # init
  def __init__(self, pin):
    self.pin = digitalio.DigitalInOut(pin)
    self.pin.pull = digitalio.Pull.UP
    self.state = self.pin.value
    self.last = 0
    self.elapsed = 0
    
  # detect sensor
  def detect(self):
    read = self.pin.value
    if read != self.state:
      self.state = read
      aux = time_ms()
      if read == 1:
        if self.last != 0:
          self.elapsed = aux - self.last
        self.last = aux
        return True
    return False

  # return time since previous detection
  def getElapsed(self):
    return self.elapsed

# Stepper connections
stepper = Stepper([board.GP2, board.GP3, board.GP4, board.GP5])

# Sensor
sensor = HallSensor(board.GP16)

# LED
led = digitalio.DigitalInOut(board.GP25)
led.direction = digitalio.Direction.OUTPUT
led.value = False

# main loop
while True:
  # choose a random speed
  delay = random.randrange(1300, 3000)/1000
  print ("Delay = {} ms".format(delay))
  changeTime = time_ms() + 30000
  while time_ms() < changeTime:
    stepper.onestep()
    if sensor.detect():
      led.value = True
      elapsed = sensor.getElapsed()
      if elapsed != 0:
        print ("RPM = {}".format(60000 // elapsed))
      time.sleep(delay/1000)
      led.value = False
    else:
      time.sleep(delay/1000)
