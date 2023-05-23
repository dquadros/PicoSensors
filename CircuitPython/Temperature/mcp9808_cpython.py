# MCP9808 Example

import board
from busio import I2C
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
    return val

# Decode temperature from sensor
def decodeTemp(val):
    sign = val & 0x1000
    val = val & 0x1FFF
    if sign != 0:
        val = val - 0x2000
    return val/16

i2c = I2C(sda=board.GP16, scl=board.GP17)

# Write 16 bit value to a register
def writeReg16(reg, val):
    data = bytearray([reg,val >> 8, val & 0xFF])
    i2c.try_lock()
    i2c.writeto(MCP9808_ADDR, data)
    i2c.unlock()

# Read 16 bit value from a register
def readReg16(reg):
    selreg = bytearray([reg])
    val = bytearray([0,0])
    i2c.try_lock()
    i2c.writeto_then_readfrom(MCP9808_ADDR, selreg, val)
    i2c.unlock()
    return (val[0] << 8) + val[1]

# Check manufacture and device IDs
print ('Manufacturer: {:04X}'.format(readReg16(MCP9808_MANID)))
devID = readReg16(MCP9808_DEVID);
print ('Device: {:02X} rev {}'.format(devID >>8, devID&0xFF))

# Set limits for the Alert output
writeReg16(MCP9808_CRIT,encodeTemp(30.0))
writeReg16(MCP9808_UPPER,encodeTemp(23.0))
writeReg16(MCP9808_LOWER,encodeTemp(20.0))
writeReg16(MCP9808_CONFIG,0x0008)

# Main loop: read temperature
while True:
    sleep(0.5)
    print ('Temperature = {}C'.format(
        decodeTemp(readReg16(MCP9808_TA))))
