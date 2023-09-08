# Mifare RFID Example with PN532

import board
import busio
import re
from adafruit_pn532.i2c import PN532_I2C
from adafruit_pn532.adafruit_pn532 import MIFARE_CMD_AUTH_A

# Init PN532
i2c = busio.I2C(sda=board.GP16,scl=board.GP17)
pn532 = PN532_I2C(i2c, debug=False)
ic, ver, rev, support = pn532.firmware_version
print('Found PN532 with firmware version: {0}.{1}'.format(ver, rev))
pn532.SAM_configuration()

# Card key and App mark
mark = bytes([ 0x00, 0x11, 0x22, 0x33, 0x44, 0x66, 0x66, 0x77,
         0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF])
cardKey = bytes([ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ]) # factory default


# Wait for a card in range
def waitForCard():
    print ('Present a card')
    while True:
        uid = pn532.read_passive_target(timeout=0.5)
        if uid is None:
            continue
        print("Card ID: {}".format(
            hex(int.from_bytes(uid,"little")).upper()))
        return uid

# Write on card
def writeToCard(param):
    uid = waitForCard()
    block = int(param[1])*4
    msg = bytes((param[2]+16*' '),'utf-8')[0:16]
    if pn532.mifare_classic_authenticate_block(uid, block, MIFARE_CMD_AUTH_A, cardKey) and \
       pn532.mifare_classic_write_block(block, mark) and \
       pn532.mifare_classic_write_block(block+1, msg):
           print ('Written.')
    else:
        print ('Write error!')

# Read from card
def readFromCard(param):
    uid = waitForCard()
    block = int(param[1])*4
    if pn532.mifare_classic_authenticate_block(uid, block, MIFARE_CMD_AUTH_A, cardKey):
        data = pn532.mifare_classic_read_block(block)
        if data == mark:
            msg = pn532.mifare_classic_read_block(block+1)
            if msg != None:
                print ('Msg: '+msg.decode('utf-8'))
                return
        elif data != None:
            print ('No message on sector!')
            return
    print ('Read error!')

# Laco Principal
cmdRead = "^([lL]) ([1-9])$"
cmdWrite = "^([gG]) ([1-9]) (.+)$"
while True:
    # Read command
    print()
    cmd = input("Command? ")
    m = re.search(cmdRead, cmd)
    if m != None:
        readFromCard(m.groups())
    else:
        m = re.search(cmdWrite, cmd)
        if m != None:
            writeToCard(m.groups())
        else:
            print ('Unknown command')
