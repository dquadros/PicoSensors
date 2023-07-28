# iButton Example

from machine import Pin
import onewire
from time import sleep

# Init OnewWire bus
ow = onewire.OneWire(Pin(16))

# Main Loop
last = None
while True:
    if ow.reset():
        # Got a presence pulse, scan for devices
        sensors = ow.scan()
        # Make sure it is a valid iButton
        if (len(sensors) > 0) and (sensors[0] != last) and \
          (ow.crc8(sensors[0]) == 0) and (sensors[0][0] == 1):
            # Extract and print ID
            last = sensors[0]
            id = ''
            for i in range (6,0,-1):
                id = id + '{:02X}'.format(last[i])
            print('ID = '+id)
            sleep(2)
    else:
        last = None
        sleep(.5)
