# MCP9808 Example

from machine import I2C,Pin
from time import sleep

# MCP9808 I2C Address
MCP9808_ADDR = 0x18

# MCP9808 Registers
MCP9808_CONFIG = 1
MCP9808_UPPER = 2  
MCP9808_LOWER = 3
MCP9808_CRIT = 4
MCP9808_TA = 5
MCP9808_MANID = 6
MCP9808_DEVID = 7
MCP9808_RESOL = 8

# Encode temperature for sensor
def encodeTemp(temp):
    if temp >= 0:
        val =  int(temp/0.25) << 2
    else:
        val = (2048 + int(temp/0.25)) << 2
    return bytearray([val >> 8, val & 0xFF])

# Decode temperature from sensor
def decodeTemp(val):
    sign = val & 0x1000
    val = val & 0x1FFF
    if sign != 0:
        val = val - 0x2000
    return val/16

i2c = I2C(0, sda=Pin(16), scl=Pin(17))

# Read 16-bit register
def readReg16(reg):
    data = i2c.readfrom_mem(MCP9808_ADDR,reg,2)
    return (data[0] << 8) + data[1]

# Check manufacture and device IDs
print ('Manufacturer: {:04X}'.format(readReg16(MCP9808_MANID)))
devID = readReg16(MCP9808_DEVID);
print ('Device: {:02X} rev {}'.format(devID >>8, devID&0xFF))

# Set limits for the Alert output
i2c.writeto_mem(MCP9808_ADDR,MCP9808_CRIT,encodeTemp(30.0))
i2c.writeto_mem(MCP9808_ADDR,MCP9808_UPPER,encodeTemp(23.0))
i2c.writeto_mem(MCP9808_ADDR,MCP9808_LOWER,encodeTemp(20.0))
i2c.writeto_mem(MCP9808_ADDR,MCP9808_CONFIG,b'\x00\x08')

# Main loop: read temperature
while True:
    sleep(0.5)
    print ('Temperature = {}C'.format(
        decodeTemp(readReg16(MCP9808_TA))))