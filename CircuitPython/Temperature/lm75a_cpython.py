# LM75A Example

import board
from busio import I2C
from time import sleep

# LM75A I2C Address
LM75_ADDR = 0x48

# LM75A Registers
LM75_TEMP = 0  
LM75_CONF = 1  
LM75_THYST = 2  
LM75_TOS = 3

i2c = I2C(sda=board.GP16, scl=board.GP17)

# Encode temperature for sensor
def encodeTemp(temp):
    if temp >= 0:
        return int(temp / 0.5) << 7
    else:
        return (512 + int(temp/0.5)) << 7

# Decode temperature from sensor
def decodeTemp(val):
    val = val >> 5
    if val >= 1024:
        return (val-2048)*0.125
    else:
        return val*0.125

# Write 8 bit value to a register
def writeReg8(reg, val):
    data = bytearray([reg, val & 0xFF])
    i2c.try_lock()
    i2c.writeto(LM75_ADDR, data)
    i2c.unlock()

# Write 16 bit value to a register
def writeReg16(reg, val):
    data = bytearray([reg, val >> 8, val & 0xFF])
    i2c.try_lock()
    i2c.writeto(LM75_ADDR, data)
    i2c.unlock()

# Read 16 bit value from a register
def readReg16(reg):
    selreg = bytearray([reg])
    val = bytearray([0,0])
    i2c.try_lock()
    i2c.writeto_then_readfrom(LM75_ADDR, selreg, val)
    i2c.unlock()
    return val

# Configure sensor and set limits for the OS output
writeReg8(LM75_CONF,0)
writeReg16(LM75_TOS,encodeTemp(22.5))
writeReg16(LM75_THYST,encodeTemp(20.0))

# Main loop: read temperature
while True:
    sleep(0.5)
    data = readReg16(LM75_TEMP)
    temp = decodeTemp((data[0] << 8) + data[1])
    print ('Temperature = {}C'.format (temp))
