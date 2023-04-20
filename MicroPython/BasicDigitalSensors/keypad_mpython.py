# Keypad Example

from machine import Pin,Timer
from time import sleep

class Keypad:
    
  # init
  def __init__(self, rowPins, columnPins, debounce=5):
    self.nr = len(rowPins)
    self.nc = len(columnPins)
    self.debounce = debounce
    self.rows = [Pin(pin, Pin.IN) for pin in rowPins]
    self.columns = [Pin(pin, Pin.IN, Pin.PULL_UP) for pin in columnPins]
    self.curRow = 0
    self.previous = [[[0] for _ in range(self.nc)] for _ in range(self.nr)]
    self.count = [[[self.debounce] for _ in range(self.nc)] for _ in range(self.nr)]
    self.validated = [[[0] for _ in range(self.nc)] for _ in range(self.nr)]
    self.keys = []

  # check next row
  def checkRow(self):
    # set row to LOW
    pin = self.rows[self.curRow]
    pin.init(mode=Pin.OUT)
    pin.low()
    # read columns and check for changes
    for col in range(self.nc):
      read = self.columns[col].value()
      if read == self.previous[self.curRow][col]:
        if self.count[self.curRow][col] != 0:
          self.count[self.curRow][col] = self.count[self.curRow][col] - 1
          if self.count[self.curRow][col] == 0:
            # reading validated
            if read != self.validated[self.curRow][col]:
              self.validated[self.curRow][col] = read
              if read == 0:
                  # keypress detected
                  self.keys.append((self.curRow, col))
      else:
        # restart validation
        self.previous[self.curRow][col] = read
        self.count[self.curRow][col] = self.debounce
    # return row to input
    pin.init(mode=Pin.IN)
    # move to next row
    self.curRow = self.curRow + 1
    if self.curRow == self.nr:
      self.curRow = 0

  # return next key pressed
  # None if none
  def getKey(self):
      if len(self.keys) == 0:
        return None
      else:
        return self.keys.pop(0)

pad = Keypad([0, 1, 2, 3], [4, 5, 6])

# timer interrupt handler
def scanKeypad(timer):
  pad.checkRow()
    
Timer(mode=Timer.PERIODIC, period=10, callback=scanKeypad)

codes = [['1', '2', '3'], ['4', '5', '6'], ['7', '8', '9'], ['*', '0', '#']]

# main loop
while True:
  key = pad.getKey()
  if key is None:
    sleep(5)
  else:
    print('{} -> {}'.format(key, codes[key[0]][key[1]]))
  