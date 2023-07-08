# MMA8452 Example

'''
CircuitPython Class for MMA8452 sensor 

Adapted form Sparkfun Arduino library
https://github.com/sparkfun/SparkFun_MMA8452Q_Arduino_Library

'''
import struct

class MMA8452():
    # Constructor
    def __init__(self, i2c=None, addr=0x1C):
        # check parameters
        if i2c is None:
            raise ValueError("I2C not specified")
        else:
            self.i2c = i2c
        self.addr = addr

        # registers
        self.STATUS_MMA8452Q = 0x00
        self.OUT_X_MSB = 0x01
        self.OUT_X_LSB = 0x02
        self.OUT_Y_MSB = 0x03
        self.OUT_Y_LSB = 0x04
        self.OUT_Z_MSB = 0x05
        self.OUT_Z_LSB = 0x06
        self.SYSMOD = 0x0B
        self.INT_SOURCE = 0x0C
        self.WHO_AM_I = 0x0D
        self.XYZ_DATA_CFG = 0x0E
        self.HP_FILTER_CUTOFF = 0x0F
        self.PL_STATUS = 0x10
        self.PL_CFG = 0x11
        self.PL_COUNT = 0x12
        self.PL_BF_ZCOMP = 0x13
        self.P_L_THS_REG = 0x14
        self.FF_MT_CFG = 0x15
        self.FF_MT_SRC = 0x16
        self.FF_MT_THS = 0x17
        self.FF_MT_COUNT = 0x18
        self.TRANSIENT_CFG = 0x1D
        self.TRANSIENT_SRC = 0x1E
        self.TRANSIENT_THS = 0x1F
        self.TRANSIENT_COUNT = 0x20
        self.PULSE_CFG = 0x21
        self.PULSE_SRC = 0x22
        self.PULSE_THSX = 0x23
        self.PULSE_THSY = 0x24
        self.PULSE_THSZ = 0x25
        self.PULSE_TMLT = 0x26
        self.PULSE_LTCY = 0x27
        self.PULSE_WIND = 0x28
        self.ASLP_COUNT = 0x29
        self.CTRL_REG1 = 0x2A
        self.CTRL_REG2 = 0x2B
        self.CTRL_REG3 = 0x2C
        self.CTRL_REG4 = 0x2D
        self.CTRL_REG5 = 0x2E
        self.OFF_X = 0x2F
        self.OFF_Y = 0x30
        self.OFF_Z = 0x31
        
        # scales
        self.SCALE_2G = 2
        self.SCALE_4G = 4
        self.SCALE_8G = 8
        
        # data rates
        self.ODR_800 = 0
        self.ODR_400 = 1
        self.ODR_200 = 2
        self.ODR_100 = 3
        self.ODR_50 = 4
        self.ODR_12 = 5
        self.ODR_6 = 6
        self.ODR_1 = 7    
        
        # orientations
        self.PORTRAIT_U = 0
        self.PORTRAIT_D = 1
        self.LANDSCAPE_R = 2
        self.LANDSCAPE_L = 3
        self.LOCKOUT = 0x40

        # system mode
        self.SYSMOD_STANDBY = 0
        self.SYSMOD_WAKE = 1
        self.SYSMOD_SLEEP = 2
        
    # Sensor init
    def begin(self):
        if self.readRegister(self.WHO_AM_I) != 42:
            return False
        self.scale = self.SCALE_2G
        self.odr = self.ODR_800
        self.setScale(self.scale)
        self.setDataRate(self.odr)
        self.setupPL() # Portrait / Landscape detection
        self.setupTap(0x80, 0x80, 0x80) # disable
        return True
        
    # Get raw acceleration
    # data in registers: hhhhhhhh llll0000
    def getRawAcel(self):
        acel = struct.unpack(">hhh", self.readRegisters (self.OUT_X_MSB, 6))
        return [a >> 4 for a in acel]

    # Get scaled acceleration 
    def getCalclulatedAcel(self):
        return [float(a)/float(1 << 11)*float(self.scale) for a in self.getRawAcel()]
    
    # Test is new data available
    def available(self):
        return (self.readRegister(self.STATUS_MMA8452Q) & 0x08) != 0
    
    # Selects scale
    def setScale(self, fsr):
        if not fsr in (self.SCALE_2G, self.SCALE_4G, self.SCALE_8G):
            raise ValueError("Invalid scale")
        
        # Scale can only be changed in standby mode
        if self.isActive():
            self.standby()
    
        # Update config
        cfg = self.readRegister(self.XYZ_DATA_CFG)
        cfg = (cfg & 0xFC) | (fsr >> 2)
        self.writeRegister(self.XYZ_DATA_CFG, cfg)
        
        # Goes to active mode
        self.active()
    
    # Selects data rate (ODR)
    def setDataRate(self, odr):
        if not odr in (self.ODR_800, self.ODR_400, self.ODR_200, self.ODR_100,
                       self.ODR_50, self.ODR_12, self.ODR_6, self.ODR_1):
            raise ValueError("Invalid data rate")

        # can only be changed in standby mode
        if self.isActive():
            self.standby()
    
        # Update config
        ctrl = self.readRegister(self.CTRL_REG1)
        ctrl = (ctrl & 0xC7) | (odr << 3)
        self.writeRegister(self.CTRL_REG1, ctrl)
        
        # Goes to active mode
        self.active()

    # Set tap detection
    def setupTap(self, xThs, yThs, zThs):
        # can only be changed in standby mode
        if self.isActive():
            self.standby()
    
        # Update config
        temp = 0x40
        if (xThs & 0x80) == 0:
            temp |= 0x03
            self.writeRegister(self.PULSE_THSX, xThs)
        if (yThs & 0x80) == 0:
            temp |= 0x0C
            self.writeRegister(self.PULSE_THSY, yThs)
        if (zThs & 0x80) == 0:
            temp |= 0x30
            self.writeRegister(self.PULSE_THSZ, zThs)
        self.writeRegister(self.PULSE_CFG, temp)
        self.writeRegister(self.PULSE_TMLT, 0x30)
        self.writeRegister(self.PULSE_LTCY, 0xA0)
        self.writeRegister(self.PULSE_WIND, 0xFF)
        
        # Goes to active mode
        self.active()

    # Read tap status
    def readTap(self):
        tapStat = self.readRegister(self.PULSE_SRC)
        if (tapStat & 0x80) != 0:
            return tapStat & 0x7F
        else:
            return 0

    # Set portrait/landscape detection
    def setupPL(self):
        # can only be changed in standby mode
        if self.isActive():
            self.standby()
    
        # Update config
        self.writeRegister(self.PL_CFG, self.readRegister(self.PL_CFG) | 0x40)
        self.writeRegister(self.PL_COUNT, 0x50)
        
        # Goes to active mode
        self.active()
        
    # Read portrait/landscape status
    def readPL(self):
        plStat = self.readRegister(self.PL_STATUS)
        if (plStat & 0x40) != 0:
            return self.LOCKOUT
        else:
            return (plStat & 0x06) >> 1
    
    # Orientation tests
    def isRight(self):
        return self.redPL() == self.LANDSCAPE_R

    def isLeft(self):
        return self.redPL() == self.LANDSCAPE_L
    
    def isUp(self):
        return self.redPL() == self.LANDSCAPE_U
    
    def isDown(self):
        return self.redPL() == self.LANDSCAPE_D
    
    def isFlat(self):
        return self.redPL() == self.LOCKOUT
    
    # Enter standby mode
    def standby(self):
        c = self.readRegister(self.CTRL_REG1)
        self.writeRegister(self.CTRL_REG1, c & 0xFE)
        
    # Enter active mode
    def active(self):
        c = self.readRegister(self.CTRL_REG1)
        self.writeRegister(self.CTRL_REG1, c | 0x01)
        
    # Check if active
    def isActive(self):
        state = self.readRegister(self.SYSMOD) & 0x03
        return state != self.SYSMOD_STANDBY
        
    # Read a register
    def readRegister(self, reg):
        selreg = bytearray([reg])
        data = bytearray(1)
        i2c.try_lock()
        i2c.writeto_then_readfrom(self.addr, selreg, data)
        i2c.unlock()
        return data[0]
    
    # Read multiple registers
    def readRegisters(self, reg, nregs):
        selreg = bytearray([reg])
        result = bytearray(nregs)
        i2c.try_lock()
        i2c.writeto_then_readfrom(self.addr, selreg, result)
        i2c.unlock()
        return result
    
    # Write into a register
    def writeRegister(self, reg, value):
        i2c.try_lock()
        i2c.writeto(self.addr, bytearray([reg, value]))
        i2c.unlock()

if __name__ == "__main__":
    '''
        Quick test
    '''
    import board
    from digitalio import DigitalInOut, Pull
    from busio import I2C
    from time import sleep
    
    i2c = I2C(sda=board.GP16, scl=board.GP17)
    sensor = MMA8452(i2c)
    if not sensor.begin():
        print ("Sensor not found")
    else:
        sensor.active()
        print ("Raw:")
        for i in range(0, 5):
            while not sensor.available:
                pass
            print (sensor.getRawAcel())
            sleep(3)
        print ("Scaled:")
        for i in range(0, 5):
            while not sensor.available:
                pass
            print (sensor.getCalclulatedAcel())
            sleep(3)
        print ("Position:")
        sensor.setupPL()
        oldpos = -1
        while True:
            sleep (0.3)
            newpos = sensor.readPL()
            if newpos == oldpos:
                continue
            oldpos = newpos
            if newpos == sensor.LOCKOUT:
                print ("Laying down")
            elif newpos == sensor.LANDSCAPE_R:
                print ("Landscape facing right")
            elif newpos == sensor.LANDSCAPE_L:
                print ("Landscape facing left")
            elif newpos == sensor.PORTRAIT_U:
                print ("Portrait facing up")
            elif newpos == sensor.PORTRAIT_D:
                print ("Portrait facing down")
            else:
                print ("Invalid position: "+str(newpos))