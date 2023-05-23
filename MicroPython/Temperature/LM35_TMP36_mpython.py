# LM35D and TMP36 Temperature Sensor Example
from machine import ADC
from time import sleep

# Connections (-1 if not used)
pinLM35 = 26
pinTMP36 = 27

if pinLM35 != -1:
    adcLM35 = ADC(pinLM35)
if pinTMP36 != -1:
    adcTMP36 = ADC(pinTMP36)

# returns the voltage in an ADC pin
# averages 10 readings
def readADC(adc):
    sum = 0
    for i in range(10):
        sum = sum+adc.read_u16()
    return (sum*0.33)/65536

# Main Loop
while True:
    tempLM35 = 0
    if pinLM35 != -1:
        vLM35 = readADC(adcLM35)
        tempLM35 = vLM35/0.01
    tempTMP36 = 0
    if pinTMP36 != -1:
        vTMP36 = readADC(adcTMP36)
        tempTMP36 = 25.0 + (vTMP36-0.75)/0.01
    print("LM35 = {:.1f}C  TMP36 = {:.1f}C".format(tempLM35, tempTMP36))
    sleep(2)
