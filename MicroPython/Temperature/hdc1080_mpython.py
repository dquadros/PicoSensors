# HDC1080 Example

from machine import I2C,Pin
from time import sleep

# HDC1080 I2C Address
HDC_ADDR = 0x40

# HDC1080 Registers
HDC_TEMP = 0
HDC_HUM = 1
HDC_CONF = 2
HDC_MANID = 0xFE
HDC_DEVID = 0xFF  

i2c = I2C(0, sda=Pin(16), scl=Pin(17))

# Read 16-bit register
def readReg16(reg):
    data = i2c.readfrom_mem(HDC_ADDR,reg,2)
    return (data[0] << 8) + data[1]

# Check manufacture and device IDs
print ('Manufacturer: {:04X}'.format(readReg16(HDC_MANID)))
print ('Device: {:04X}'.format(readReg16(HDC_DEVID)))

while True:
    i2c.writeto(HDC_ADDR, bytearray([HDC_TEMP]))
    sleep (0.02)
    data = i2c.readfrom(HDC_ADDR, 4)
    r = (data[0] << 8) + data[1]
    temp = r*165/65536 - 40.0
    r = (data[2] << 8) + data[3]
    humid = r*100/65536
    print ('Temperature: {:.1f}C Humidity: {:.0f}%'.format(temp, humid))
    sleep(2)

