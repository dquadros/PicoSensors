# AHT10 Temperature Sensor Example

from machine import I2C,Pin
from time import sleep_ms

# AHT10 I2C Address
AHT_ADDR = 0x38

i2c = I2C(0, sda=Pin(16), scl=Pin(17))

# Read ADH10 status
def getStatus():
    resp = i2c.readfrom (AHT_ADDR, 1)
    return resp[0]

# Check if a calibration is needed
status = getStatus()
if (status & 0x08) == 0:
    print ('Calibrating')
    i2c.writeto(AHT_ADDR, b'\xE1\x08\x00')
    sleep_ms(10)

# Main Loop
while True:
    # Start convertion
    i2c.writeto(AHT_ADDR, b'\xAC\x33\x00')
    # Wait convertion
    sleep_ms(80)
    # Get result
    resp = i2c.readfrom (AHT_ADDR, 6)
    # Decode result
    umid = (resp[1] << 12) + (resp[2] << 4) + (resp[3] >> 4)
    umid = (umid / 0x100000) * 100.0
    temp = ((resp[3] & 0x0F) << 16) + (resp[4] << 8) + resp[5]
    temp = (temp / 0x100000) * 200.0 - 50.0
    # Show result
    print ('Humidity = {:.1f}%  Temperature = {:.1f}C'.format(umid, temp))
    # Wait between readings
    sleep_ms(2000)
