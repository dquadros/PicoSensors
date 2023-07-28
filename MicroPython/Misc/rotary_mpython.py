# Rotary Encoder Example

import rp2 
from rp2 import PIO, asm_pio
from machine import Pin

# PIO Program
@asm_pio(fifo_join=PIO.JOIN_RX)
def ENCODER_PIO():
    wrap_target()
    label('loop')
    # Copy the state-change timer from OSR,
    # decrement it, and save it back
    mov (y,osr)
    jmp(y_dec,'osr_dec')
    
    label('osr_dec')
    mov (osr, y)

    # Read the state of both encoder pins and check
    # if they are different from the last state
    jmp (pin, 'enc_a_was_high')
    mov (isr, null)
    jmp ('read_enc_b')
    
    label('enc_a_was_high')
    set (y, 1)
    mov (isr, y)
    
    label('read_enc_b')
    in_(pins, 1)
    mov (y, isr)
    jmp (x_not_y, 'state_changed') [1]
    wrap ()
    
    label('state_changed')
    # Put the last state and the timer value into
    # ISR alongside the current state, and push that
    # state to the system. Then override the last
    # state with the current state
    in_ (x, 2)
    mov (x, invert(osr))
    in_ (x, 28)
    push (noblock)
    mov (x, y)

    # Perform a delay to debounce switch inputs
    set (y, 29) [19]
    label('debounce_loop')
    jmp (y_dec, 'debounce_loop') [15]

    # Initialise the timer
    mov (y, invert(null))
    jmp (y_dec, 'y_dec')
    label('y_dec')
    mov (osr, y)
    jmp ('loop') [1]

# Set up pins
pin_A = Pin(16, Pin.IN, Pin.PULL_UP)
pin_B = Pin(17, Pin.IN, Pin.PULL_UP)

# Init PIO
sm = rp2.StateMachine(0)
sm.init(ENCODER_PIO,freq=500000,
        in_base=pin_B,in_shiftdir=PIO.SHIFT_LEFT,push_thresh=1,
        jmp_pin=pin_A)
state = (pin_A.value() << 1) | pin_B.value()
sm.exec("set(x,"+str(state)+")")
sm.active(1)
hist = 0
val = 50
print(val)

while True:
    state = (sm.get() & 0xC0000000) >> 30
    hist = ((hist & 0x3F) << 2) | state
    if hist == 0x4B:
        if  val < 100:
            val = val+1
            print(val)
    elif hist == 0xE1:
        if val > 0:
            val = val-1
            print(val)  
