# HC-SR04 Ultrasonic Sensor Example

import utime
import rp2 
from rp2 import PIO, asm_pio
from machine import Pin

# PIO Program
@asm_pio(set_init=rp2.PIO.OUT_LOW,autopush=False)
def ULTRA_PIO():
    # wait for a request and save timeout
    pull()
    mov (x,osr)
    
    # send a 10 us (20 cycles) pulse
    set(pins,1) [19]
    set(pins,0)
    
    # wait for the start of the echo pulse
    wait(1,pin,0)
    
    # wait for the end of the echo pulse
    # decrements X every 2 cycles (1us)
    label('dowait')
    jmp(pin,'continue')
    jmp('end')    
    label('continue')
    jmp(x_dec,'dowait')
    
    # return pulse duration
    label('end')
    mov(isr,x)
    push()

# Set up pins
trigger = Pin(17, Pin.OUT)
echo = Pin(16, Pin.IN)
sm = rp2.StateMachine(0)

# Do measurements
while True:
    sm.init(ULTRA_PIO,freq=2000000,set_base=trigger,in_base=echo,jmp_pin=echo)
    sm.active(1)
    sm.put(300000)
    val = sm.get()
    sm.active(0)
    if val < 300000:
        ellapsed = 300000 - val
        distance = (ellapsed * 0.0343) / 2
        print('Distance = {0:.1f} cm'.format(distance))
        utime.sleep_ms(1000)
    else:
        print('Timeout')
  