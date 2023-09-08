# Mifare RFID Example with MFRC522

from machine import Pin
from mfrc522 import MFRC522
import time
import utime
import re
 
# Connections
pinSCK = 18
pinMISO = 16
pinMOSI = 19
pinCS = 17
pinRST = 20

# Card key and App mark
cardKey = [ 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF ] # factory default
mark = [ 0x00, 0x11, 0x22, 0x33, 0x44, 0x66, 0x66, 0x77,
         0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF]

# Inicia MFRC522
card = MFRC522(spi_id=0,sck=pinSCK,miso=pinMISO,mosi=pinMOSI,
               cs=pinCS,rst=pinRST)
card.init()

# Wait for a card in range
def waitForCard():
    print ('Present a card')
    while True:
        utime.sleep_ms(50)
        (stat, tag_type) = card.request(card.REQIDL)
        if stat == card.OK:
            (stat, uid) = card.SelectTagSN()
            if stat == card.OK:
                print("Card ID: {}".format(
                    hex(int.from_bytes(bytes(uid),"little",False)).upper()))
                return uid

# Write on card
def writeToCard(param):
    uid = waitForCard()
    sector = int(param[1])
    msg = bytes((param[2]+16*' '),'utf-8')[0:16]
    stat = card.writeSectorBlock(uid, sector, 0, mark, cardKey)
    if stat != card.OK:
        print ('Write error!')
    else:
        stat = card.writeSectorBlock(uid, sector, 1, msg, cardKey)
        if stat != card.OK:
            print ('Write error!')
        else:
            print ('Written.')
    card.stop_crypto1()
    
# Le do cartao
def readFromCard(param):
    uid = waitForCard()
    sector = int(param[1])
    (stat, blockMark) = card.readSectorBlock(uid, sector, 0, cardKey)
    if stat != card.OK:
        print ('Read error!')
    elif blockMark != mark:
        print ('No message on sector!')
    else:
        (stat, blockMsg) = card.readSectorBlock(uid, sector, 1, cardKey)
        if (stat != card.OK):
            print ('Read error!')
        else:
            print ('Msg: '+bytes(blockMsg).decode('utf-8'))
    card.stop_crypto1()
    
# Main Loop
cmdRead = "^([rR]) ([1-9])$"
cmdWrite = "^([wW]) ([1-9]) (.+)$"
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

