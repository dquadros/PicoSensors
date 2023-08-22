#  Fingerprint Sensor Example
import board
import digitalio
from busio import UART
from time import sleep
from fingersensor_cpython import FingerSensor

# Init
ledR = digitalio.DigitalInOut(board.GP13)
ledG = digitalio.DigitalInOut(board.GP14)
ledB = digitalio.DigitalInOut(board.GP15)
ledR.direction = digitalio.Direction.OUTPUT
ledG.direction = digitalio.Direction.OUTPUT
ledB.direction = digitalio.Direction.OUTPUT
ledR.value = False
ledG.value = False
ledB.value = False
uart = UART(tx=board.GP16, rx=board.GP17, baudrate=9600*6, bits=8, parity=None, stop=1)
finger = FingerSensor(uart)
sleep(0.1)
c = finger.count()
if c > 0:
    print ('Erasing {} fingerprints'.format(c))
    if finger.clear():
        print ('Success')

# Read fingerprint and create template
def captureFeature(numbuf):
    while True:
        ledB.value = True
        print ('Place finger on sensor')
        while not finger.capture():
            sleep(0.05)
        ledB.value = False
        print ('Image captured')
        ok = finger.createFeature(numbuf)
        print ('Remove finger from sensor')
        sleep(3)
        if ok:
            print('Feature created')
            return
        ledR.value = True
        print ('Bad image, try again')
        sleep(1)
        ledR.value = False

# Enroll fingerprint
def enroll():
    first = True
    while True:
        if not first:
            captureFeature(1)
        captureFeature(2)
        if not finger.createModel():
            ledR.value = True
            first = False
            print ('Images do not match, try again')
            sleep(1)
            ledR.value = False
            continue
        pos = finger.count()
        if finger.store(1,pos):
            ledG.value = True
            print ('Fingerprint stored at {}'.format(pos))
            sleep(1)
            ledG.value = False
            return
        else:
            ledR.value = True
            print ('Error {} while storing fingerprint'.format(finger.lastResponse()))
            sleep(1)
            ledR.value = False

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
        ledG.value = True
        print ('Fingerprint {} identified'.format(pos))
        sleep(1)
        ledG.value = False
