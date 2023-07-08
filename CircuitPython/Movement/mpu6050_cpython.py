# MPU6050 Sensor Example
import board
from busio import I2C
from time import sleep
import adafruit_mpu6050

# Set up sensor
i2c = I2C(sda=board.GP16, scl=board.GP17)
sensor = adafruit_mpu6050.MPU6050(i2c)

# Main loop
G = 9.8
while True:
    x,y,z = sensor.acceleration
    print ('Accel X:{:.1f}g y:{:.1f}g Z:{:.1f}g'.format(x/G, y/G, z/G))
    x,y,z = sensor.gyro
    print ('Gyro X:{:.1f} y:{:.1f} Z:{:.1f}'.format(x,y,z))
    print ('Temp: {:.1f}C'.format(sensor.temperature))
    print ()
    sleep(2)
