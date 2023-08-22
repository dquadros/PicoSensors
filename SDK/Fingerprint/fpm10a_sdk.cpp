/**
 * @file fpm10a_sdk.c
 * @author Daniel Quadros (dqsoft.blogspot@gmail.com)
 * @brief Simple driver for FPM10A fingerprint sensor
 * @version 1.0
 * @date 2023-08-22
 * 
 * @copyright Copyright (c) 2023, Daniel Quadros
 * 
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "hardware/uart.h"
#include "fpm10a_sdk.h"

// Constructor
FPM10A::FPM10A(uart_inst_t *serial) {
  uart = serial;
}

// Start communication
void FPM10A::begin() {
  uart_init (uart, 9600*6);
  uart_set_format (uart, 8, 1, UART_PARITY_NONE);
  sleep_ms (500);
  response = RESP_OK;
}

// Returns last reponse
uint8_t FPM10A::lastResponse() {
  return response;
}

// Read configuration
bool FPM10A::leSysParam (SYSPARAM *param) {
  CMDNOPARAM cmd;
  cmd.cmd = CMD_READSYSPARAM;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  if (getResponse() && (resp.response == RESP_OK)) {
    memcpy (param, &resp.d.sysparam, sizeof(SYSPARAM));
    return true;
  } else {
    return false;
  }
}

// Returns the number of stored fingerprints
// -1 if something goes wrong
int FPM10A::count() {
  CMDNOPARAM cmd;
  cmd.cmd = CMD_TEMPLATECOUNT;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  if (getResponse() && (resp.response == RESP_OK)) {
    return (resp.d.templcount[0] << 8)+ resp.d.templcount[1];
  } else {
    return -1;
  }
}

// Captura a imagem de uma digital
bool FPM10A::capture () {
  CMDNOPARAM cmd;
  cmd.cmd = CMD_GENIMG;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  return getResponse() && (resp.response == RESP_OK);
}

// Convert current fingerprint image into a feature
// and put in CharBuffer[numBuf] (1 or 2)
bool FPM10A::createFeature (uint8_t numBuf) {
  CMDIMG2TZ cmd;
  cmd.cmd = CMD_IMG2TZ;
  cmd.bufno = numBuf;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  return getResponse() && (resp.response == RESP_OK);
}

// Create a model (template) from the two features in CharBuffer
bool FPM10A::createModel () {
  CMDNOPARAM cmd;
  cmd.cmd = CMD_REGMODEL;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  return getResponse() && (resp.response == RESP_OK);
}

//  Save model in CharBuffer[numBuf] (1 or 2)
//  into position pos (0 to 149)
bool FPM10A::store (uint8_t numBuf, int pos) {
  CMDSTORE cmd;
  cmd.cmd = CMD_STORE;
  cmd.bufno = numBuf;
  cmd.pos[0] = pos >> 8;
  cmd.pos[1] = pos & 0xFF;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  return getResponse() && (resp.response == RESP_OK);
}


// Load model at position 'pos' (0 to 149) in the library
// into CharBuffer 'numBuf' (1 ou 2)
bool FPM10A::load (uint8_t numBuf, int pos) {
  CMDLOAD cmd;
  cmd.cmd = CMD_LOAD;
  cmd.bufno = numBuf;
  cmd.pos[0] = pos >> 8;
  cmd.pos[1] = pos & 0xFF;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  return getResponse() && (resp.response == RESP_OK);
}

// Compare fingerprints in CharBuffer 1 and 2
 bool FPM10A::match () {
  CMDNOPARAM cmd;
  cmd.cmd = CMD_MATCH;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  return getResponse() && (resp.response == RESP_OK);
}

// Search for the fingerprint corresponding to the feature at CharBuffer[numBuf]
// Return position if found, -1 otherwise
int FPM10A::search (uint8_t numBuf) {
  CMDSEARCH cmd;
  int ndigitals = count();
  cmd.cmd = CMD_SEARCH;
  cmd.bufno = numBuf;
  cmd.start[0] = 0;
  cmd.start[1] = 0;
  cmd.qty[0] = ndigitals >> 8;
  cmd.qty[1] = ndigitals & 0xFF;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  if (getResponse() && (resp.response == RESP_OK)) {
    return (resp.d.search.pos[0] << 8) + resp.d.search.pos[1];
  } else {
    return -1;
  }
}

// Clear all stored fingerprints
bool FPM10A::clear () {
  CMDNOPARAM cmd;
  cmd.cmd = CMD_EMPTY;
  sendCmd ((uint8_t *) &cmd, sizeof(cmd));
  return getResponse() && (resp.response == RESP_OK);
}

// Send command to sensor
void FPM10A::sendCmd (uint8_t *cmd, int size) {
  // fill header and send
  header.startHi = START >> 8;
  header.startLo = START & 0xFF;
  header.addr3 = 0xFF;  // default address
  header.addr2 = 0xFF;
  header.addr1 = 0xFF;
  header.addr0 = 0xFF;
  header.type = 0x01; // command
  header.lenHi = (size+2) >> 8;
  header.lenLo = (size+2) & 0xFF;
  uart_write_blocking (uart, (const uint8_t *) &header, sizeof(header));

  // compute checksum
  uint16_t chksum = header.type + header.lenHi + header.lenLo;
  for (int i = 0; i < size; i++) {
    chksum += cmd[i];
  }

  // send command
  uart_write_blocking (uart, (const uint8_t *) cmd, size);
  
  // send checksum
  uart_putc_raw (uart, chksum >> 8);
  uart_putc_raw (uart, chksum & 0xFF);
}

// Read response from sensor
bool FPM10A::getResponse() {
  enum { START1, START2, ADDR, TAG, LEN1, LEN2, DATA, CHECK1, CHECK2 } state = START1;
  uint16_t checksum;
  uint8_t *p = (uint8_t *) &resp;
  int size;
  int i;
  bool ok = false;
  while (true) {
    int c = uart_getc(uart);
    //printf (" %02X", c);
    *p++ = c;
    switch (state) {
      case START1:
        if (c == (START >> 8)) {
          state = START2;
        } else {
          p = (uint8_t *) &resp; // start again
        }
        break;
      case START2:
        if (c == (START & 0xFF)) {
          state = ADDR;
          i = 0;
        } else {
          state = START1;
          p = (uint8_t *) &resp; // start again
        }
        break;
      case ADDR:
        if (++i == 4) {
          state = TAG;
        }
        break;
      case TAG:
        checksum = c;
        state = LEN1;
        break;
      case LEN1:
        checksum += c;
        size = c << 8;
        state = LEN2;
        break;
      case LEN2:
        checksum += c;
        size += c - 2;  // discount checksum
        state = DATA;
        i = 0;
        break;
      case DATA:
        checksum += c;
        if (++i == size) {
          state = CHECK1;
        }
        break;
      case CHECK1:
        ok = c == (checksum >> 8);
        state = CHECK2;
        break;
      case CHECK2:
        ok = ok && (c == (checksum & 0xFF));
        if (ok) {
          response = resp.response;
        }
        //printf("\n");
        return ok;
    }
  }
}
