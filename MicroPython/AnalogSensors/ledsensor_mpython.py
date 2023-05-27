# LED as Light Sensor Example

from machine import Pin, ADC
from time import sleep

sensor = ADC(Pin(26))

def val():
    sum = 0
    for i in range(20):
        sum = sum + sensor.read_u16();
    return sum//20

while True:
    print (val() // 100)
    sleep(2)
  
    
