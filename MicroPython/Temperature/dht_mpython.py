# DHT11/DHT22 Example

import utime
import rp2 
from rp2 import PIO, asm_pio
from machine import Pin

# Connections, -1 = not used
pinDHT11 = 16
pinDHT22 = 17

# PIO Program
@asm_pio(set_init=(PIO.OUT_HIGH),autopush=True, push_thresh=8) 
def DHT_PIO():
    # wait for a start from the program
    pull()
     
    # keep data low for the time provided
    set(pindirs,1)              #set pin to output  
    set(pins,0)                 #set pin low
    mov (x,osr)
    label ('waitx')
    nop() [25] 
    jmp(x_dec,'waitx')          # wait for count*26/clock
      
    # starts reading response
    set(pindirs,0)              # change pin to input
    wait(1,pin,0)               # wait for it to come back to high
    wait(0,pin,0)               # wait for starting pulse
    wait(1,pin,0)
    wait(0,pin,0)               # wait for start of first bit
 
    # read data bits
    label('readdata')
    wait(1,pin,0)               # wait for data high
    set(x,20)                   # x is timeout
    label('countdown')
    jmp(pin,'continue')         # continue conting if data high
     
    # pin low while counting -> bit 0
    set(y,0)                 
    in_(y, 1)                   # put a 0 in result
    jmp('readdata')             # read next bit
     
    # pin still high
    label('continue')
    jmp(x_dec,'countdown')      # decrement count
 
    # timeout -> bit 1
    set(y,1)                  
    in_(y, 1)                   # put a 1 in the result
    wait(0,pin,0)               # wait for low
    jmp('readdata')             # read next bit
 
DHT11 = 0
DHT22 = 1
 
class DHT:
 
    # Construtor
    # dataPin: pin connected to the DHT
    # model:   DHT11 ou DHT22
    # smID:    state machine id
    def __init__(self, dataPin, model, smID=0):
        self.dataPin = dataPin
        self.model = model
        self.smID = smID
        self.sm = rp2.StateMachine(self.smID)
        self.lastreading = 0
        self.data=[]
     
    # execute a reading
    def read(self):
        data=[]
        self.sm.init(DHT_PIO,freq=1400000,set_base=self.dataPin,in_base=self.dataPin,jmp_pin=self.dataPin)
        self.sm.active(1)
        if self.model == DHT11:
            self.sm.put(969)     # wait 18 ms
        else:
            self.sm.put(54)      # wait 1 ms
        for i in range(5):       # read 5 bytes
            data.append(self.sm.get())
        self.sm.active(0)
        total=0
        for i in range(4):
            total=total+data[i]
        if data[4] == (total & 0xFF):
            # checksum ok, save the data
            self.data = data
            self.lastreading = utime.ticks_ms()
            return True
        else:
            return False
 
    # read or use last data
    def getData(self):
        # make sure we have some data
        while len(self.data) == 0:
            if not self.read():
                utime.sleep_ms(2000)
             
        # new read only if more than 2 seconds from previous
        now = utime.ticks_ms()
        if self.lastreading > now:
            self.lastreading = now  # count wraped around
        if (self.lastreading+2000) < now:
            self.read()
     
    # return humidity
    def humidity(self):
        self.getData()
        if self.model == DHT11:
            return self.data[0] + self.data[1]*0.1
        else:
            return ((self.data[0] << 8) + self.data[1]) * 0.1
 
    # return temperature
    def temperature(self):
        self.getData()
        if self.model == DHT11:
            return self.data[2] + self.data[3]*0.1
        else:
            s = 1
            if (self.data[2] & 0x80) == 1:
                s = -1
            return s * (((self.data[2] & 0x7F) << 8) + self.data[3]) * 0.1
 
#main program

if pinDHT11 != -1:
    dht11_data = Pin(pinDHT11, Pin.IN, Pin.PULL_UP)
    dht11 = DHT(dht11_data, DHT11, 0)
    
if pinDHT22 != -1:
    dht22_data = Pin(pinDHT22, Pin.IN, Pin.PULL_UP)
    dht22 = DHT(dht22_data, DHT22, 1)
 
while True:
    if pinDHT11 != -1:
        print("DHT11 Humidity: %.1f%%, Temperature: %.1fC" % 
          (dht11.humidity(), dht11.temperature()))
    if pinDHT22 != -1:
        print("DHT22 Humidity: %.1f%%, Temperature: %.1fC" % 
          (dht22.humidity(), dht22.temperature()))
    utime.sleep_ms(3000)
