# LED as Light Sensor Example

from machine import Pin, ADC
from time import sleep

sensor = ADC(Pin(26))

def val():
    soma = 0
    for i in range(20):
        soma = soma + sensor.read_u16();
    return soma//20

while True:
    print (val() // 100)
    sleep(2)
  
    
