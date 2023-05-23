# HDC1080 Example

import board
from digitalio import DigitalInOut, Pull
from busio import I2C
from time import sleep

# HDC1080 I2C Address
HDC_ADDR = 0x40

# HDC1080 Registers
HDC_TEMP = 0
HDC_HUM = 1
HDC_CONF = 2
HDC_MANID = 0xFE
HDC_DEVID = 0xFF  

i2c = I2C(sda=board.GP16, scl=board.GP17)

def readReg16(reg):
    selreg = bytearray([reg])
    val = bytearray([0,0])
    i2c.try_lock()
    i2c.writeto_then_readfrom(HDC_ADDR, selreg, val)
    i2c.unlock()
    return (val[0]<<8)+val[1]

# Check manufacture and device IDs
print ('Manufacturer: {:04X}'.format(readReg16(HDC_MANID)))
print ('Device: {:04X}'.format(readReg16(HDC_DEVID)))

while True:
    i2c.try_lock()
    i2c.writeto(HDC_ADDR, bytearray([HDC_TEMP]))
    i2c.unlock()
    sleep (0.02)
    i2c.try_lock()
    data = bytearray([0,0,0,0])
    i2c.readfrom_into(HDC_ADDR, data)
    r = (data[0] << 8) + data[1]
    temp = r*165/65536 - 40.0
    r = (data[2] << 8) + data[3]
    humid = r*100/65536
    print ('Temperature: {:.1f}C Humidity: {:.0f}%'.format(temp, humid))
    sleep(2)

