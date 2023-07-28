# Load cell Example

import board
import digitalio
from busio import I2C
from i2c_lcd import lcd_pcf8574
from hx711.hx711_pio import HX711_PIO
from time import sleep

# Init button
button = digitalio.DigitalInOut(board.GP15)
button.pull = digitalio.Pull.UP

# Init display
i2c = I2C(sda=board.GP16, scl=board.GP17)
lcd = lcd_pcf8574(i2c)
lcd.init()
lcd.backlightOn()

# Wait for a key press
def keyPress(msg):
    lcd.displayClear()
    lcd.displayWrite(0,0,msg)
    lcd.displayWrite(1,0,'Press button')
    while button.value:
        sleep(0.1)
    sleep(0.1) # debounce
    lcd.displayWrite(1,0,'Release button')
    while not button.value:
        sleep(0.1)
    sleep(0.1) # debounce
    lcd.displayWrite(1,0,'Wait          ')
    
# Initial calibration
sensor = HX711_PIO(board.GP12, board.GP13)
keyPress('Empty scale')
sensor.tare()
keyPress('Put 1kg')
sensor.determine_scalar(1.0)
if sensor.read() < 0:
    sensor.scalar = -sensor.scalar

# Main loop
lcd.displayClear()
lcd.displayWrite(0,0,'Scale Ready')
while True:
    weight = sensor.read()
    lcd.displayWrite(1,0,'{:7.3f}kg'.format(weight))
    sleep(0.5)
