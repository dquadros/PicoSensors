# Thermistor Example
import math
from machine import Pin, ADC
from time import sleep

# Circuit Parameters
Vcc = 3.3
R = 2200.0

# Thermistor at ADC0
sensor = ADC(0)

# Returns the thermistor resistance
# uses a mean of 20 ADC readings
def val():
    sum = 0
    for i in range(20):
        sum = sum + sensor.read_u16();
    v = (Vcc*sum)/(20*65536.0)
    return R*(Vcc-v)/v

# Gets a reference (temperature + resistance)
def getRef():
    while True:
        x = input('Temperature: ')
        try:
            t = float(x)
            r = val()
            print('Resistance: {:.0f}'.format(r))
            print()
            return 273.0 + t, r
        except ValueError:
            print ('Invalid format!')

# Compute beta from two references
print('Reference 1:')
t1, rt1 = getRef()
print('Reference 2:')
t2, rt2 = getRef()
beta = math.log(rt1/rt2)/((1/t1)-(1/t2))
print('Beta = {:.2f}'.format(beta))
rx = rt1 * math.exp(-beta/t1)

# Main loop
while True:
    rt = val()
    t = beta / math.log(rt/rx)
    print ('Temperature: {:.1f}'.format(t-273.0))
    sleep(1)
    
