# QMC5883L Example
import board
from digitalio import DigitalInOut, Pull
from busio import I2C
from time import sleep
from math import pi, atan2

# Class to acess sensor
class QMC5883L:
    
    def __init__(self, i2c):
        self.i2c = i2c
        self.QMC_ADDR = 0x0D
        self.QMC_REG_XL = 0
        self.QMC_REG_ST = 6
        self.QMC_REG_CR1 = 9
        self.QMC_REG_SR = 11
        i2c.try_lock()
        i2c.writeto(self.QMC_ADDR, bytearray([self.QMC_REG_SR, 0x01]))
        i2c.writeto(self.QMC_ADDR, bytearray([self.QMC_REG_CR1, 0x81]))
        i2c.unlock()
        
    def getHeading(self):
        selreg = bytearray([self.QMC_REG_ST])
        st = bytearray([0])
        i2c.try_lock()
        i2c.writeto_then_readfrom(self.QMC_ADDR, selreg, st)
        i2c.unlock()
        while (st[0] & 1) == 0:
            sleep(0.01)
            i2c.try_lock()
            i2c.writeto_then_readfrom(self.QMC_ADDR, selreg, st)
            i2c.unlock()
        selreg = bytearray([self.QMC_REG_XL])
        data = bytearray([0,0,0,0,0,0])
        i2c.try_lock()
        i2c.writeto_then_readfrom(self.QMC_ADDR, selreg, data)
        i2c.unlock()
        x = data[0] + (data[1] << 8);
        y = data[2] + (data[3] << 8);
        if (x & 0x8000) != 0:
            x = x - 0x10000
        if (y & 0x8000) != 0:
            y = y - 0x10000
        angle = atan2(float(y),float(x))
        return (angle*180)/pi + 180

# Test Program
i2c = I2C(sda=board.GP16, scl=board.GP17)
compass = QMC5883L(i2c)
while True:
    print ('Heading: {:.1f}'.format(compass.getHeading()))
    sleep(2)
