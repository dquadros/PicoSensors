// MIFARE MFRC522 RFID reader Example

#include <SPI.h>
#include <MFRC522.h>
 
// RFID reader control
const byte RST_PIN = 20;  // reset pin
const byte SS_PIN = 17;   // select pin
MFRC522 mfrc522(SS_PIN, RST_PIN);
 
// Card access keys
typedef enum { FACTORY, OUR_APP, OTHER } KEY_TYPE;
MFRC522::MIFARE_Key defaultKey;
MFRC522::MIFARE_Key appKeyA;
MFRC522::MIFARE_Key appKeyB;
KEY_TYPE cardKey;  // key type of current card
 
// Selects the sector and block to be used to store balance
const byte SECTOR = 2;
const byte VALUE_BLK = 8;
const byte CFG_BLK = 11;
 
// Initial and debit values
const int32_t INITIAL_VALUE = 30;
const int32_t DEBIT_VALUE = 7;
 
// Init
void setup() {
  while (!Serial) {
    delay(100);
  }

  // Init serial port
  Serial.begin (115200);
 
  // Init RFID reader
  SPI.begin();
  mfrc522.PCD_Init();
 
  // Init card keys
  for (byte i = 0; i < 6; i++) {
    defaultKey.keyByte[i] = 0xFF;
  }
  appKeyA.keyByte[0] = 'S';  appKeyA.keyByte[1] = 'e';  appKeyA.keyByte[2] = 'c';
  appKeyA.keyByte[3] = 'r';  appKeyA.keyByte[4] = 'e';  appKeyA.keyByte[5] = 't';
  appKeyB.keyByte[0] = '1';  appKeyB.keyByte[1] = '2';  appKeyB.keyByte[2] = '3';
  appKeyB.keyByte[3] = '4';  appKeyB.keyByte[4] = '5';  appKeyB.keyByte[5] = '6';
}
 
// Main loop
void loop() {
  // Request operation
  Serial.print ("(I)nit card, (D)ebit, or (R)estore factory settings?");
  int c;
  while (Serial.read() != -1) {
    delay(10);
  }
  do {
    c = Serial.read();
    c = toupper(c);
  } while (strchr("IDR", c) == NULL);
  char cmd[2] = "";
  cmd[0] = (char) c;
  Serial.println (cmd);
 
  // Asks to present a card to the reader
  if (askForCard()) {
    // Execute operation
    if (c == 'I') {
      initCard();
    } else if (c == 'D') {
      useCard();
    } else if (c == 'R') {
      resetCard();
    }
    mfrc522.PICC_HaltA();
    mfrc522.PCD_StopCrypto1();
  }
  Serial.println();
}
 
// Prepare card for use
void initCard() {
  MFRC522::StatusCode status;
   
  // Authenticate with the B key
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, CFG_BLK, 
      (cardKey == FACTORY)? &defaultKey : &appKeyB, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Authentication error!");
    return;
  }
 
  // Write balance sector with the proper format
  if (!setupValue(VALUE_BLK)) {
    return;
  }
 
  // Reconfigure the sector to use our keys and the needed access conditions
  // 1st block 000 standard config, can access with A or B keys
  // 2nd block 110 value, read/decrement with A or B key, write/decrement with B key only
  // 3rd block 000 standard config, can access with A or B keys
  // 4rd block 011 writ with B key only, key A can only read access bits
  byte cfgBuffer[16];
  memset (cfgBuffer, 0, 16);
  memcpy (cfgBuffer, &appKeyA, 6);
  memcpy (cfgBuffer+10, &appKeyB, 6);
  mfrc522.MIFARE_SetAccessBits(&cfgBuffer[6], 0, 6, 0, 3);
  status = mfrc522.MIFARE_Write(CFG_BLK, cfgBuffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Error while configuring card!");
  }

  Serial.println ("Card initialized");
  cardKey = OUR_APP;

  // Authenticate with new B key
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, CFG_BLK, 
      &appKeyB, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Authentication error!");
    return;
  }
 
  status = mfrc522.MIFARE_Increment(VALUE_BLK, INITIAL_VALUE);
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Error loading initial value!");
    return;
  }
  status = mfrc522.MIFARE_Transfer(VALUE_BLK);
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Error saving new balance!");
    return;
  }
 
  // Shows initial balance
  showBalance();
}
 
// Use DEBIT_VALUE "credits"
void useCard() {
  MFRC522::StatusCode status;
 
  if (cardKey != OUR_APP) {
    Serial.println ("Card not initialized!");
    return;
  }
   
  // Authenticates with A key
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, CFG_BLK, 
      &appKeyA, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Authentication error!");
    return;
  }
 
  // Check if enough balance
  int32_t value;
  status = mfrc522.MIFARE_GetValue(VALUE_BLK, &value);
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Read error!");
    return;
  }
  if (value < DEBIT_VALUE) {
    Serial.println ("Insuficient balance!");
    return;
  }
 
  // Update balance
  status = mfrc522.MIFARE_Decrement(VALUE_BLK, DEBIT_VALUE);
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Debit error!");
    return;
  }
  status = mfrc522.MIFARE_Transfer(VALUE_BLK);
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Error while saving new balance!");
    return;
  }
 
  // Shows new balance
  showBalance();
}
 
// Shows current balance
void showBalance() {
  MFRC522::StatusCode status;
 
  if (cardKey != OUR_APP) {
    Serial.println ("Card not initialized!");
    return;
  }
   
  // Authenticates with A key
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, CFG_BLK, 
      &appKeyA, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Authentication error!");
    return;
  }
 
  // Read value
  int32_t value;
  status = mfrc522.MIFARE_GetValue(VALUE_BLK, &value);
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Read Error!");
    return;
  }
  Serial.print ("Balance: ");
  Serial.println (value);
}
 
// Reset card to factory settings
void resetCard() {
  MFRC522::StatusCode status;
 
  // Authenticates with B key
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, CFG_BLK, 
      (cardKey == FACTORY)? &defaultKey : &appKeyB, &(mfrc522.uid));
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Authentication error!");
    return;
  }
   
  // Reconfigura o setor para a configuração padrão
  byte cfgBuffer[16];
  memset (cfgBuffer, 0, 16);
  memcpy (cfgBuffer, &defaultKey, 6);
  memcpy (cfgBuffer+10, &defaultKey, 6);
  mfrc522.MIFARE_SetAccessBits(&cfgBuffer[6], 0, 0, 0, 1);
  status = mfrc522.MIFARE_Write(CFG_BLK, cfgBuffer, 16);
  if (status != MFRC522::STATUS_OK) {
    Serial.println ("Error while configuring!");
  }
  Serial.println ("Card reset to factory settings");
}
 
// Wait for a card or a key
bool askForCard() {
  while (Serial.read() != -1)
    ;
  Serial.println ("Present a card (or press a key to cancel)");
  while (true) {
    if (Serial.read() != -1) {
      return false;
    }
    if (!mfrc522.PICC_IsNewCardPresent())
      continue;
    if (mfrc522.PICC_ReadCardSerial())
      break;
  }

  Serial.print ("ID:");
  for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
  }
  Serial.println();
   
  Serial.print(F("Card type: "));
  MFRC522::PICC_Type tipo = mfrc522.PICC_GetType(mfrc522.uid.sak);
  Serial.println(mfrc522.PICC_GetTypeName(tipo));
 
  // Check type
  if ( (tipo == MFRC522::PICC_TYPE_MIFARE_MINI) ||
       (tipo == MFRC522::PICC_TYPE_MIFARE_1K) ||
       (tipo == MFRC522::PICC_TYPE_MIFARE_4K) ) {
    cardKey = checkKey();
    return cardKey != OTHER;
  }
  Serial.println ("Card not supported!");
  return false;
}
 
// Checks what type of key is used by the card
KEY_TYPE checkKey() {
  MFRC522::StatusCode status;
 
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, CFG_BLK, 
      &defaultKey, &(mfrc522.uid));
  if (status == MFRC522::STATUS_OK) {
    Serial.println ("Accepts factory key");
    return FACTORY;
  }
 
  // Reset communication to try another key
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
  mfrc522.PICC_IsNewCardPresent();
  mfrc522.PICC_ReadCardSerial();
   
  status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, CFG_BLK, 
      &appKeyA, &(mfrc522.uid));
  if (status == MFRC522::STATUS_OK) {
    Serial.println ("Accepts our key");
    return OUR_APP;
  }
   
  Serial.println ("Unknown key!");
  return OTHER;
}
 
// Set up a block for value operations
bool setupValue(byte block) {
  MFRC522::StatusCode status;
  byte valueBlock[] = {
      0, 0, 0, 0,
      255, 255, 255, 255,
      0, 0, 0, 0,
      block, (byte) ~block, block, (byte) ~block };
  status = mfrc522.MIFARE_Write(block, valueBlock, 16);
  if (status != MFRC522::STATUS_OK) {
      Serial.print("Write error!");
      return false;
  }
  return true;
}
