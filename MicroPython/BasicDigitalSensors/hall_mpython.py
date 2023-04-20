# Hall Effect Sensor Example

from machine import Pin
import time
import random

#   Stepper motor control class
class Stepper():
  # set motor control pins
  def setpins(self, val=[0,0,0,0]):
    for i in range(len(self.pins)):
      self.pins[i].value(val[i])
 
  # init
  def __init__(self, pins=None):
    if pins is None:
      raise ValueError("Must specify pins!")
    if len(pins) != 4:
      raise ValueError("There must be 4 pins")
    self.pins = pins
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
    self.pin = pin
    self.state = self.pin.value()
    self.last = 0
    self.elapsed = 0
    
  # detect sensor
  def detect(self):
    read = self.pin.value()
    if read != self.state:
      self.state = read
      aux = time.ticks_ms()
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
pins = [
  Pin(2, Pin.OUT),
  Pin(3, Pin.OUT),
  Pin(4, Pin.OUT),
  Pin(5, Pin.OUT)
]
stepper = Stepper(pins)

# Sensor
sensor = HallSensor (Pin(16, Pin.IN, Pin.PULL_UP))

# LED
led = Pin(25, Pin.OUT)
led.off()

# main loop
while True:
  # choose a random speed
  delay = random.randrange(1300, 3000)
  print ("Delay = {} ms".format(delay/1000))
  changeTime = time.ticks_ms() + 30000
  while time.ticks_ms() < changeTime:
    stepper.onestep()
    if sensor.detect():
      led.on()
      elapsed = sensor.getElapsed()
      if elapsed != 0:
        print ("RPM = {}".format(60000 // elapsed))
      time.sleep_us(delay)
      led.off()
    else:
      time.sleep_us(delay)
