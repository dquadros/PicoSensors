# 125kHz RFID Reader Example
import board
import digitalio
import pwmio
from busio import UART
from time import sleep, monotonic_ns

def time_ms():
    return monotonic_ns() // 1000000

# Class to control a buzzer
class Buzzer:
    def __init__(self, pin):
        self.pinBuzzer = digitalio.DigitalInOut(pin)
        self.pinBuzzer.direction = digitalio.Direction.OUTPUT
        self.pinBuzzer.value = False
        
    def beep(self):
        self.pinBuzzer.value = True
        sleep(.3)
        self.pinBuzzer.value = False

# Class to check a button
class Button:
    def __init__(self, pin, debounce=20):
        self.pinButton = digitalio.DigitalInOut(pin)
        self.pinButton.pull = digitalio.Pull.UP
        self.pressed = False
        self.debounce = debounce
        self.last = self.pinButton.value
        self.lastTime = time_ms()
        
    def released(self):
        val = self.pinButton.value
        if val != self.last:
            # reading changed
            self.last = val
            self.lastTime = time_ms()
        elif (val == 0) != self.pressed:
            dif = time_ms() - self.lastTime
            if dif > self.debounce:
                # update button state
                self.pressed = val == 0
                return not self.pressed
        return False
        
# Class to control a servomotor
class Servo:
    def __init__(self, pin, time0deg=0.6, time180deg=2.4):
        self.pwmServo = pwmio.PWMOut(pin, duty_cycle=0, frequency=50)
        self.time0deg = time0deg
        self.time180deg = time180deg
        
    def pos(self, angle):
        ontime = self.time0deg+(self.time180deg-self.time0deg)*angle/180
        val = ontime * 65535/20
        self.pwmServo.duty_cycle = int(val)
        sleep(0.3)
        self.pwmServo.duty_cycle = 0

# Class to get RFID reader messages
class RFID:
    def __init__(self, pin):
        self.uart = UART(tx=None, rx=pin, baudrate=9600, bits=8, parity=None, stop=1)
        self.last = ''
        self.last_read = 0
        self.bufRx = bytearray(14)
        self.pos = 0
        
    def read(self):
        if self.uart.in_waiting == 0:
            if (self.last != '') and ((time_ms()- self.last_read) > 1000):
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
led = digitalio.DigitalInOut(25)
led.direction = digitalio.Direction.OUTPUT
buzzer = Buzzer(board.GP2)
button = Button(board.GP16)
servo = Servo(board.GP17)
rfid = RFID(board.GP13)

# Blink LED, "close door" and beep
led.value = True
buzzer.beep()
servo.pos(0)
led.value = False

# Main Loop
autorized = set()
enroll = False
closeTime = None
while True:
    if (closeTime != None) and (time_ms() > closeTime):
        servo.pos(0)
        closeTime = None
    if button.released():
        # pressed and released button
        enroll = True
    tag = rfid.read()
    if tag != None:
        if enroll:
            if tag in autorized:
                print ('Tag {} already authorized'.format(tag))
            else:
                led.value = True
                autorized.add(tag)
                print ('Tag {} authorized'.format(tag))
                sleep(0.3)
                led.value = False
            enroll = False
        else:
            if tag in autorized:
                print ('Tag {} authorized'.format(tag))
                servo.pos(180) # "open door"
                closeTime = time_ms()+3000
            else:
                print ('Tag {} NOT authorized'.format(tag))
                buzzer.beep()
