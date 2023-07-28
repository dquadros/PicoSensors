# Simple I2C (PCF8574) Alphanumeric LCD Driver

from time import sleep

class lcd_pcf8574(object):
    
    # set lcd signals
    def write8574(self):
        self.bufTx[0] = self.curValue
        self.i2c.try_lock()
        self.i2c.writeto(self.addr, self.bufTx)
        self.i2c.unlock()

    # constructor
    def __init__(self, i2c, addr=0x27, bitRS=0, bitRW=1, bitE=2, bitBL=3, bitD4=4, bitD5=5, bitD6=6, bitD7=7):
        # save config
        self.mskRS = 1 << bitRS
        self.mskRW = 1 << bitRW
        self.mskE = 1 << bitE
        self.mskBL = 1 << bitBL
        self.mskD4 = 1 << bitD4
        self.mskD5 = 1 << bitD5
        self.mskD6 = 1 << bitD6
        self.mskD7 = 1 << bitD7
        
        # PCF8574 set up
        self.addr = addr
        self.i2c = i2c
        self.bufTx = bytearray(1)
        self.curValue = 0x00
        self.write8574()
        
        # constantes
        self.LOW = 0
        self.HIGH = 1
        self.CMD = self.LOW
        self.DADO = self.HIGH
        self.CMD_CLS = 0x01
        self.CMD_DISPON = 0x0C
        self.CMD_POSCUR = 0x80
        self.CMD_FUNCTIONSET = 0x20
        self.LCD_4BITMODE = 0x00
        self.LCD_2LINE = 0x08
        self.LCD_5x8DOTS = 0x00

    # controla sinal RS
    def setRS(self, valor):
        if valor == self.LOW:
            self.curValue = self.curValue & ~self.mskRS
        else:
            self.curValue = self.curValue | self.mskRS
        self.write8574()

    # controla sinal RW
    def setRW(self, valor):
        if valor == self.LOW:
            self.curValue = self.curValue & ~self.mskRW
        else:
            self.curValue = self.curValue | self.mskRW
        self.write8574()

    # controla sinal E (enable)
    def setE(self, valor):
        if valor == self.LOW:
            self.curValue = self.curValue & ~self.mskE
        else:
            self.curValue = self.curValue | self.mskE
        self.write8574()

    # controla backlight
    def setBL(self, valor):
        if valor == self.LOW:
            self.curValue = self.curValue & ~self.mskBL
        else:
            self.curValue = self.curValue | self.mskBL
        self.write8574()

    # controla os pinos de dados (D4 a D7)
    def setDado(self, nib):
        self.curValue = self.curValue & ~(self.mskD4 | self.mskD5 | self.mskD6 | self.mskD7)
        if (nib & 8) != 0:
            self.curValue = self.curValue | self.mskD7
        if (nib & 4) != 0:
            self.curValue = self.curValue | self.mskD6
        if (nib & 2) != 0:
            self.curValue = self.curValue | self.mskD5
        if (nib & 1) != 0:
            self.curValue = self.curValue | self.mskD4
        self.write8574()

    # envia um byte para o display
    def writeByte(self, rs, dado):
        self.setRS(rs)
        self.setE(self.HIGH)
        self.setDado (dado >> 4)
        self.setE(self.LOW)
        self.setE(self.HIGH)
        self.setDado (dado)
        self.setE(self.LOW)

    # envia um comando para o display
    def writeCmd(self, cmd):
        self.writeByte(self.CMD, cmd)

    # envia um caracter para o display
    def writeChar(self, chr):
        self.writeByte(self.DADO, chr)
    
    # inicia o display
    def init(self):
        # para o caso de ter acabado de ligar
        sleep(0.1)
        # vamos sempre fazer escrita
        self.setRW(self.LOW)
        # sequencia para garantir modo 4 bits
        self.writeCmd(0x03)
        sleep(0.005)
        self.writeCmd(0x03)
        sleep(0.001)
        self.writeCmd(0x03)
        sleep(0.001)
        self.writeCmd(0x02)
        sleep(0.001)
        # configura o display
        self.writeCmd(self.CMD_FUNCTIONSET | self.LCD_4BITMODE | self.LCD_2LINE | self.LCD_5x8DOTS);
        sleep(0.001)
        # limpa a tela e liga o display
        self.writeCmd(self.CMD_CLS)
        sleep(0.002)
        self.video = self.CMD_DISPON
        self.writeCmd(self.video)

    # liga o backlight
    def backlightOn(self):
        self.setBL(self.HIGH)

    # desliga o backlight
    def backlightOff(self):
        self.setBL(self.LOW)

    # liga o display
    def displayOn(self):
        self.video = self.video | 4
        self.writeCmd(self.video)
    
    # desliga o display
    def displayOff(self):
        self.video = self.video & 0xFB
        self.writeCmd(self.video)

    # posiciona o cursor
    def setPosCursor(self, lin, col):
        self.ender = col
        if lin == 1:
            self.ender = self.ender + 0x40
        self.writeCmd (self.CMD_POSCUR + self.ender)

    # controla se cursor está visivel e se pisca
    def setModeCursor(self, visivel, pisca):
        self.video = self.video & 0xFC
        if visivel:
            self.video |= 2
        if pisca:
            self.video |= 1
        self.writeCmd(self.video)
        
    # limpa o display
    def displayClear(self):
        self.writeCmd(self.CMD_CLS)
        sleep(0.002)
    
    # escreve um texto no display
    def displayWrite(self, lin, col, texto):
        self.setPosCursor(lin, col)
        for chr in texto:
            self.writeChar(ord(chr))
