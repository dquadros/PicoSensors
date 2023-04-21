# Keypad Example

import digitalio
import board
from time import sleep

class Keypad:
    
  # init
  def __init__(self, rowPins, columnPins, debounce=5):
    self.nr = len(rowPins)
    self.nc = len(columnPins)
    self.debounce = debounce
    self.rows = [digitalio.DigitalInOut(pin) for pin in rowPins]
    self.columns = [digitalio.DigitalInOut(pin) for pin in columnPins]
    for digPin in self.columns:
        digPin.pull = digitalio.Pull.UP
    self.curRow = 0
    self.previous = [[[False] for _ in range(self.nc)] for _ in range(self.nr)]
    self.count = [[[self.debounce] for _ in range(self.nc)] for _ in range(self.nr)]
    self.validated = [[[False] for _ in range(self.nc)] for _ in range(self.nr)]
    self.keys = []

  # check next row
  def checkRow(self):
    # set row to LOW
    pin = self.rows[self.curRow]
    pin.direction = digitalio.Direction.OUTPUT
    pin.value = False
    # read columns and check for changes
    for col in range(self.nc):
      read = self.columns[col].value
      if read == self.previous[self.curRow][col]:
        if self.count[self.curRow][col] != 0:
          self.count[self.curRow][col] = self.count[self.curRow][col] - 1
          if self.count[self.curRow][col] == 0:
            # reading validated
            if read != self.validated[self.curRow][col]:
              self.validated[self.curRow][col] = read
              if read == False:
                  # keypress detected
                  self.keys.append((self.curRow, col))
      else:
        # restart validation
        self.previous[self.curRow][col] = read
        self.count[self.curRow][col] = self.debounce
    # return row to input
    pin.direction = digitalio.Direction.INPUT
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

pad = Keypad([board.GP0, board.GP1, board.GP2, board.GP3],
             [board.GP4, board.GP5, board.GP6])

codes = [['1', '2', '3'], ['4', '5', '6'], ['7', '8', '9'], ['*', '0', '#']]

# main loop
while True:
  pad.checkRow()
  key = pad.getKey()
  if key is None:
    sleep(0.01)
  else:
    print('{} -> {}'.format(key, codes[key[0]][key[1]]))
  