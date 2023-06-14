# BMP390 Example
from BMP3XX import BMP3XX, ULTRA_PRECISION
from machine import I2C, Pin
from time import sleep

# Class for generic I2C
class BMP390_I2C(BMP3XX):
    def __init__(self, i2c, i2c_addr=0x77):
        self._addr = i2c_addr
        self.i2c = i2c
        super(BMP390_I2C, self).__init__()

    def _write_reg(self, reg, data):
        if isinstance(data, int):
            data = bytearray([data])
        self.i2c.writeto_mem(self._addr, reg, data)

    def _read_reg(self, reg, length):
        return self.i2c.readfrom_mem(self._addr, reg, length)

# Main Program
i2c = I2C(0, sda=Pin(16), scl=Pin(17))
bmp = BMP390_I2C(i2c)
print ('Chip ID:')
if bmp.begin():
    print ()
    bmp.set_common_sampling_mode(ULTRA_PRECISION)
    while (True):
        print('Temperature {:.1f}C  Pressure {:.0f}Pa  Altitude {:.2f}m'.format(
            bmp.get_temperature, bmp.get_pressure, bmp.get_altitude))
        sleep(2)
else:
    print ('Sensor not found!')
