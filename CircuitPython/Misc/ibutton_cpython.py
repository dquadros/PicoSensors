# iButton Example

import board
from adafruit_onewire.bus import OneWireBus
from time import sleep

# Init OnewWire bus
ow = OneWireBus(board.GP16)

# Main Loop
last = None
while True:
    if ow.reset():
        # Got a presence pulse, scan for devices
        sensors = ow.scan()
        # Make sure it is a valid iButton
        if (len(sensors) > 0) and (sensors[0].rom != last) and \
          (ow.crc8(sensors[0].rom) == 0) and (sensors[0].family_code == 1):
            # Extract and print ID
            last = sensors[0].rom
            id = ''
            for i in range (6,0,-1):
                id = id + '{:02X}'.format(last[i])
            print('ID = '+id)
            sleep(2)
    else:
        last = None
        sleep(.5)

