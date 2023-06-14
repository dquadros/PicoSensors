# AHT10 Temperature Sensor Example

import board
from digitalio import DigitalInOut, Pull
from busio import I2C
from time import sleep

# AHT10 I2C Address
AHT_ADDR = 0x38

i2c = I2C(sda=board.GP16, scl=board.GP17)

# Read AHT status
def getStatus():
    resp = bytearray([0])
    i2c.try_lock()
    i2c.readfrom_into (AHT_ADDR, resp)
    i2c.unlock()
    return resp[0]

# Check if a calibration is needed
status = getStatus()
if (status & 0x08) == 0:
    print ('Calibrating')
    i2c.try_lock()
    i2c.writeto(AHT_ADDR, b'\xE1\x08\x00')
    i2c.unlock()
    sleep(0.01)

# Main loop
while True:
    # Start conversion
    i2c.try_lock()
    i2c.writeto(AHT_ADDR, b'\xAC\x33\x00')
    i2c.unlock()
    # Wait conversion
    sleep(0.08)
    # Get result
    resp = bytearray([0,0,0,0,0,0])
    i2c.try_lock()
    i2c.readfrom_into (AHT_ADDR, resp)
    i2c.unlock()
    # Decode result
    umid = (resp[1] << 12) + (resp[2] << 4) + (resp[3] >> 4)
    umid = (umid / 0x100000) * 100.0
    temp = ((resp[3] & 0x0F) << 16) + (resp[4] << 8) + resp[5]
    temp = (temp / 0x100000) * 200.0 - 50.0
    # Show result
    print ('Humidity = {:.1f}%  Temperature = {:.1f}C'.format(umid, temp))
    # Wait between readings
    sleep(2)
