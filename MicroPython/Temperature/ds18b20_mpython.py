# DS18B20 Temperature Sensor Example

from machine import Pin
import onewire
import ds18x20
from time import sleep

# Identify sensors on the OneWire bus
ow = onewire.OneWire(Pin(16))
ds = ds18x20.DS18X20(ow)
sensors = ds.scan()
for sensor in sensors:
    print(''.join(hex(i)[2:4] for i in sensor))
print()

# Main Loop
while True:
    ds.convert_temp()
    sleep(1)
    temp = ''
    for sensor in sensors:
        temp = temp + '{:.2f}C '.format(ds.read_temp(sensor))
    print (temp)
