# Hall Effect Analog Sensor Example

import analogio
import board
from time import sleep

# Sensor is at ADC0
sensor = analogio.AnalogIn(board.A0)

# Get an average of 50 readings
def val():
  sum = 0
  for i in range(50):
    sum = sum + sensor.value;
  return sum // 50

# Assume no magnetic field at the start
zero = val() 
print('Zero = {}'.format(zero))
sleep(1)
print ('Ready')

# Main loop
while True:
  field = val()
  if abs(field-zero) < 100:
      print ('No field')
  elif field > zero:
    print ('Field = S {}'.format (field-zero))
  else:
    print ('Field = N {}'.format (zero- field))
  sleep(1)

