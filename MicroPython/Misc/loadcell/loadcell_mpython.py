# Load cell Example

import rp2 
from rp2 import PIO, asm_pio
from machine import Pin, I2C
from i2c_lcd import lcd_pcf8574
from time import sleep

# PIO Program
@asm_pio(set_init=rp2.PIO.OUT_LOW,autopush=True,
         fifo_join=PIO.JOIN_RX)
def HX711_PIO():
    # wait a new conversion
    wait (1,pin,0)
    wait (0,pin,0)
    
    # read data bits
    set (x,23)
    label('readbit')
    set (pins, 1) [3]
    in_ (pins, 1)
    set (pins, 0) [3]
    jmp (x_dec, 'readbit')
    
    # add 1 pulse to set gain to 128
    set (pins,1) [4]
    set (pins,0)

# Set up pins
pin_data = Pin(12, Pin.IN)
pin_clock = Pin(13, Pin.OUT)

# Start the PIO
sm = rp2.StateMachine(0)
sm.init(HX711_PIO,
        set_base=pin_clock,
        in_base=pin_data,
        in_shiftdir=PIO.SHIFT_LEFT,
        freq=5_000_000,
        push_thresh=24)
sm.active(1)

# Clear PIO FIFO
def clearFIFO():
    while sm.rx_fifo() > 0:
        sm.get()

# Read a raw value from HX711
def hx711_raw():
    val = sm.get()
    if (val & 0x800000) != 0:
        val -= 0x100000
    return val

# Read an average
def hx711_avg(count = 10):
    sum = 0
    clearFIFO()
    for _ in range(count):
        sum += hx711_raw()
    return sum // count

# Init button
button = Pin (15, Pin.IN, Pin.PULL_UP)

# Init display
i2c = I2C(0, sda=Pin(16), scl=Pin(17))
lcd = lcd_pcf8574(i2c)
lcd.init()
lcd.backlightOn()

# Wait for a key press
def keyPress(msg):
    lcd.displayClear()
    lcd.displayWrite(0,0,msg)
    lcd.displayWrite(1,0,'Press button')
    while button.value():
        sleep(0.1)
    sleep(0.1) # debounce
    lcd.displayWrite(1,0,'Release button')
    while not button.value():
        sleep(0.1)
    sleep(0.1) # debounce
    lcd.displayWrite(1,0,'Wait          ')
    
# Initial calibration
keyPress('Empty scale')
tare = hx711_avg(50)
keyPress('Put 1kg')
scale = (hx711_avg(50) - tare)/1.0

# Main loop
lcd.displayClear()
lcd.displayWrite(0,0,'Scale Ready')
while True:
    weight = (hx711_avg() - tare)/scale
    lcd.displayWrite(1,0,'{:7.3f}kg'.format(weight))
    sleep(0.5)
