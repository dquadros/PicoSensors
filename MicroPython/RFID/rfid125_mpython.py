# 125kHz RFID Reader Example
from machine import Pin,PWM,UART
from time import sleep, ticks_ms, ticks_add, ticks_diff

def time_ms():
    return ticks_ms()

# Class to control a buzzer
class Buzzer:
    def __init__(self, pin):
        self.pinBuzzer = Pin(pin, Pin.OUT)
        self.pinBuzzer.off()
        
    def beep(self):
        self.pinBuzzer.on()
        sleep(.3)
        self.pinBuzzer.off()

# Class to check a button
class Button:
    def __init__(self, pin, debounce=20):
        self.pinButton = Pin(pin, Pin.IN, Pin.PULL_UP)
        self.pressed = False
        self.debounce = debounce
        self.last = self.pinButton.value()
        self.lastTime = time_ms()
        
    def released(self):
        val = self.pinButton.value()
        if val != self.last:
            # reading changed
            self.last = val
            self.lastTime = time_ms()
        elif (val == 0) != self.pressed:
            dif = ticks_diff(time_ms(),self.lastTime)
            if dif > self.debounce:
                # update button state
                self.pressed = val == 0
                return not self.pressed
        return False
        
# Class to control a servomotor
class Servo:
    def __init__(self, pin, time0deg=0.6, time180deg=2.4):
        self.pwmServo = PWM(Pin(pin, Pin.OUT))
        self.pwmServo.freq(50)
        self.pwmServo.duty_u16(0)
        self.time0deg = time0deg
        self.time180deg = time180deg
        
    def pos(self, angle):
        ontime = self.time0deg+(self.time180deg-self.time0deg)*angle/180
        val = ontime * 65535/20
        self.pwmServo.duty_u16(int(val))
        sleep(0.3)
        self.pwmServo.duty_u16(0)

# Class to get RFID reader messages
class RFID:
    def __init__(self, id, pin):
        self.uart = UART(id, tx=None, rx=Pin(pin), baudrate=9600, bits=8, parity=None, stop=1)
        self.last = ''
        self.last_read = 0
        self.bufRx = bytearray(14)
        self.pos = 0
        
    def read(self):
        if self.uart.any() == 0:
            ellapsed = ticks_diff(time_ms(), self.last_read)
            if (self.last != '') and (ellapsed > 1000):
                # Long time without messages, forget last tag
                self.last = ''
            return None
        rx = self.uart.read(1)
        if rx == None:
            return None
        c = rx[0]
        if self.pos==0 and c!=0x02:
            return None
        self.bufRx[self.pos] = c
        self.pos = self.pos+1
        if self.pos == 14:
            self.pos = 0
            if c == 0x03:
                # got full message
                self.last_read = time_ms()
                crc = 0
                for i in range(1,13,2):
                    x = self.bufRx[i:i+2].decode()
                    crc = crc ^ int(x,16)
                if crc == 0:
                    tag = self.bufRx[3:11].decode()
                    if tag != self.last:
                        self.last = tag
                        return tag
        return None

# Create objects
led = Pin(25, Pin.OUT)
buzzer = Buzzer(2)
button = Button(16)
servo = Servo(17)
rfid = RFID(0,13)

# 
led.on()
buzzer.beep()
servo.pos(0)
led.off()

# Main Loop
autorized = set()
cadastro = False
closeTime = None
while True:
    if (closeTime != None) and (ticks_diff(time_ms(), closeTime) > 0):
        servo.pos(0)
        closeTime = None
    if button.released():
        # pressed and released button
        cadastro = True
    tag = rfid.read()
    if tag != None:
        if cadastro:
            if tag in autorized:
                print ('Tag {} already authorized'.format(tag))
            else:
                led.on()
                autorized.add(tag)
                print ('Tag {} authorized'.format(tag))
                sleep(0.3)
                led.off()
            cadastro = False
        else:
            if tag in autorized:
                print ('Tag {} authorized'.format(tag))
                servo.pos(180) # "open door"
                closeTime = ticks_add(time_ms(), 3000)
            else:
                print ('Tag {} NOT authorized'.format(tag))
                buzzer.beep()

