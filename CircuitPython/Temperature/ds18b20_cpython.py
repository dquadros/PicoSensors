# DS18B20 Temperature Sensor Example

import board
from adafruit_onewire.bus import OneWireBus
from adafruit_ds18x20 import DS18X20
from time import sleep

# Identify devices on the OneWire bus
ow_bus = OneWireBus(board.GP16)
devices = ow_bus.scan()
sensors = []
for device in devices:
    print(''.join(hex(i)[2:4] for i in device.rom))
    if device.family_code == 0x28:
        sensors.append(DS18X20(ow_bus, device))
print()

# Main Loop
while True:
    for sensor in sensors:
        sensor.start_temperature_read()
    sleep(1)
    temp = ''
    for sensor in sensors:
        temp = temp + '{:.2f}C '.format(sensor.read_temperature())
    print (temp)

