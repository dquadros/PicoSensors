# QMC5883L Example
from machine import I2C, Pin
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
        i2c.writeto_mem(self.QMC_ADDR, self.QMC_REG_SR, b'\x01')
        i2c.writeto_mem(self.QMC_ADDR, self.QMC_REG_CR1, b'\x81')
        
    def getHeading(self):
        st = i2c.readfrom_mem(self.QMC_ADDR,self.QMC_REG_ST,1)
        while (st[0] & 1) == 0:
            sleep(0.01)
            st = i2c.readfrom_mem(self.QMC_ADDR,self.QMC_REG_ST,1)
        data = i2c.readfrom_mem(self.QMC_ADDR,self.QMC_REG_XL,6)
        x = data[0] + (data[1] << 8);
        y = data[2] + (data[3] << 8);
        if (x & 0x8000) != 0:
            x = x - 0x10000
        if (y & 0x8000) != 0:
            y = y - 0x10000
        angle = atan2(float(y),float(x))
        return (angle*180)/pi + 180

# Test Program
i2c = I2C(0, sda=Pin(16), scl=Pin(17))
compass = QMC5883L(i2c)
while True:
    print ('Heading: {:.1f}'.format(compass.getHeading()))
    sleep(2)
