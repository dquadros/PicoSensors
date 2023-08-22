#  Fingerprint Sensor Example
from machine import UART, Pin
from time import sleep
from fingersensor_mpython import FingerSensor

# Init
ledR = Pin(13, Pin.OUT)
ledG = Pin(14, Pin.OUT)
ledB = Pin(15, Pin.OUT)
ledR.off()
ledG.off()
ledB.off()
uart = UART(0, tx=Pin(16), rx=Pin(17))
finger = FingerSensor(uart)
sleep(0.5)
c = finger.count()
if c > 0:
    print ('Erasing {} fingerprints'.format(c))
    if finger.clear():
        print ('Success')

# Read fingerprint and create template
def captureFeature(numbuf):
    while True:
        ledB.on()
        print ('Place finger on sensor')
        while not finger.capture():
            sleep(0.05)
        ledB.off()
        print ('Image captured')
        ok = finger.createFeature(numbuf)
        print ('Remove finger from sensor')
        sleep(3)
        if ok:
            print('Feature created')
            return
        ledR.on()
        print ('Bad image, try again')
        sleep(1)

# Enroll fingerprint
def enroll():
    first = True
    while True:
        if not first:
            captureFeature(1)
        captureFeature(2)
        if not finger.createModel():
            ledR.on()
            first = False
            print ('Images do not match, try again')
            sleep(1)
            ledR.off()
            continue
        pos = finger.count()
        if finger.store(1,pos):
            ledG.on()
            print ('Fingerprint stored at {}'.format(pos))
            sleep(1)
            ledG.off()
            return
        else:
            ledR.on()
            print ('Error {} while storing fingerprint'.format(finger.lastResponse()))
            sleep(1)
            ledR.off()

# Main Loop
while True:
    print()
    captureFeature(1)
    print('Searching...')
    pos = finger.search(1)
    if pos == -1:
        print ('Unknown, lets enroll')
        enroll()
    else:
        ledG.on()
        print ('Fingerprint {} identified'.format(pos))
        sleep(1)
        ledG.off()
