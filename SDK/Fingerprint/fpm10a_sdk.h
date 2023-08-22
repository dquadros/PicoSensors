// Simple driver for FPM10A fingerprint sensor

#ifndef _FPM10A_H

#define _FPM10A_H

class FPM10A {
  public:

    // Responses
    static const uint8_t RESP_OK = 0;        // Success
    static const uint8_t RESP_ERRCOM = 1;    // Communication error
    static const uint8_t RESP_NOFINGER = 2;  // Finger not detected
    static const uint8_t RESP_IMGFAIL = 3;   // Could not capture image
    static const uint8_t RESP_BADIMG = 6;    // Bad image
    static const uint8_t RESP_FEWFP = 7;     // Not enough features
    static const uint8_t RESP_NOMATCH = 8;   // Fingerprints dont match
    static const uint8_t RESP_TOODIF = 10;   // Too different images
    static const uint8_t RESP_NOIMG = 21;    // No image

    // System Parameters
    typedef struct {
      uint8_t statusreg[2];
      uint8_t verify[2];
      uint8_t libsize[2];
      uint8_t seclevel[2];
      uint8_t addr[4];
      uint8_t datasize[2];
      uint8_t baud[2];
    } SYSPARAM;
  
    FPM10A (uart_inst_t *serial);
    void begin ();
    uint8_t lastResponse();
    bool leSysParam (SYSPARAM *param);
    int count ();
    bool capture ();
    bool createFeature (uint8_t numBuf);
    bool createModel ();
    bool store (uint8_t numBuf, int pos);
    bool load (uint8_t numBuf, int pos);
    bool match ();
    int search (uint8_t numBuf);
    bool clear ();
    
  private:
    // Serial object for communication
    uart_inst_t *uart;
  
    // Last response code
    uint8_t response;

    // Start of packet
    static const uint16_t START = 0xEF01;

    // Commands
    static const uint8_t CMD_GENIMG = 0x01;
    static const uint8_t CMD_IMG2TZ = 0x02;
    static const uint8_t CMD_MATCH = 0x03;
    static const uint8_t CMD_SEARCH = 0x04;
    static const uint8_t CMD_REGMODEL = 0x05;
    static const uint8_t CMD_STORE = 0x06;
    static const uint8_t CMD_LOAD = 0x07;
    static const uint8_t CMD_DELETE = 0x0C;
    static const uint8_t CMD_EMPTY = 0x0D;
    static const uint8_t CMD_READSYSPARAM = 0x0F;
    static const uint8_t CMD_TEMPLATECOUNT = 0x1D;
    
    // Packet header
    typedef struct {
      uint8_t startHi;
      uint8_t startLo;
      uint8_t addr3;
      uint8_t addr2;
      uint8_t addr1;
      uint8_t addr0;
      uint8_t type;
      uint8_t lenHi;
      uint8_t lenLo;
    } HEADER;
    HEADER header;
    
    // Commands structures
    typedef struct {
      uint8_t cmd;
    } CMDNOPARAM;
    typedef struct {
      uint8_t cmd;
      uint8_t bufno;
    } CMDIMG2TZ;
    typedef struct {
      uint8_t cmd;
      uint8_t bufno;
      uint8_t pos[2];
    } CMDSTORE;
    typedef struct {
      uint8_t cmd;
      uint8_t bufno;
      uint8_t pos[2];
    } CMDLOAD;
    typedef struct {
      uint8_t cmd;
      uint8_t bufno;
      uint8_t start[2];
      uint8_t qty[2];
    } CMDSEARCH;
    
    // Response structures
    typedef struct {
      HEADER header;
      uint8_t response;
      union {
        uint8_t data[256];
        uint8_t templcount[2];
        SYSPARAM sysparam;
        struct {
          uint8_t pos[2];
          uint8_t score[2];
        } search;
      } d;
    } RESPONSE;
    RESPONSE resp;

    void sendCmd (uint8_t *cmd, int size);
    bool getResponse ();
};

#endif