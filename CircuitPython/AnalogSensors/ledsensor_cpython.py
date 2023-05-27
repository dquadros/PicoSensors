# LED as Light Sensor Example

import analogio
import digitalio
import board
from time import sleep

sensor = analogio.AnalogIn(board.A0)

def val():
    sum = 0
    for i in range(20):
        sum = sum + sensor.value;
    return sum//20

while True:
    print (val() // 100)
    sleep(2)
  
    

