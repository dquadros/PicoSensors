// 125kHz RFID Example
#include <Servo.h>

// Class to control a buzzer
class Buzzer {
  private:
    int pinBuzzer;
  public:
    Buzzer(int pin) {
      pinBuzzer = pin;
      pinMode (pinBuzzer, OUTPUT);
      digitalWrite(pinBuzzer, LOW);
    }

    void beep() {
      digitalWrite(pinBuzzer, HIGH);
      delay(300);
      digitalWrite(pinBuzzer, LOW);
    }
};

// Class to check a button
class Button {
  private:
    int pinButton;
    bool pressed;
    int debounce;
    bool last;
    uint32_t lastTime;
 public:
    Button(int pin, int debounce = 20) {
      pinButton = pin;
      pinMode (pinButton, INPUT_PULLUP);
      pressed = false;
      this->debounce = debounce;
      last = digitalRead(pinButton) == LOW;
      lastTime = millis();
    }

  // Tests if a button was pressed and released
  bool released() {
    bool val = digitalRead(pinButton) == LOW;
    if (val != last) {
      // reading changed
      last = val;
      lastTime = millis();
    } else if (val != pressed) {
      int dif = millis() - lastTime;
      if (dif > debounce) {
        // updates button state
        pressed = val;
        return !pressed;
      }
    }
    return false;
  }
};

// Defines UART to use (Serial1 or Serial2)
#define SERIAL_RFID Serial1

// Class to get RFID reader messages
class RFID {
  private:
    byte last[9];
    uint32_t last_read;
    byte bufRx[14];
    int pos;

    inline byte decodHex(byte c) {
      if ((c >= '0') && (c <= '9')) {
        return c - '0';
      }
      if ((c >= 'A') && (c <= 'F')) {
        return c - 'A' + 10;
      }
      return 0;
    }

  public:
    RFID(int pinRx, int pinTx) {
      SERIAL_RFID.setRX(pinRx);
      SERIAL_RFID.setTX(pinTx);
      SERIAL_RFID.begin(9600, SERIAL_8N1);
      pos = 0;
      last[0] = 0;
      last_read = 0;
    }

    bool read(char *tag) {
      if (SERIAL_RFID.available() == 0) {
        uint32_t ellapsed = millis() - last_read;
        if ((last[0] != 0) && (ellapsed > 1000)) {
          // Long time without messages, forget last tag
          last[0] = 0;
        }
        return false;
      }
      int c = SERIAL_RFID.read();
      if (c < 0) {
        return false;
      }
      if ((pos == 0) && (c != 0x02)) {
        return false;
      }
      bufRx[pos++] = (byte) c;
      if (pos == 14) {
        pos = 0;
        if (c == 0x03) {
          last_read = millis();
          byte crc = 0;
          byte x;
          for (int i = 1; i < 13; i = i+2) {
            x = (decodHex(bufRx[i]) << 4) + decodHex(bufRx[i+1]);
            crc ^= x;
          }
          if (crc == 0) {
            if (memcmp (bufRx+3, last, 8) != 0) {
              memcpy(last, bufRx+3, 8);
              last[8] = 0;
              strcpy(tag, (const char *)last);
              return true;
            }
          }
        }
      }
      return false;
    }

};

// Declare and create objects
Buzzer buzzer(2);
Button button(16);
Servo servo;
RFID *rfid;

#define MAX_TAGS 10
char autorized[MAX_TAGS][9];
int nAuthorized = 0;
bool enroll = false;
uint32_t closeTime = 0;

// Init
void setup() {
  while (!Serial) {
    delay(100);
  }
  Serial.println("125kHz RFID Example\n");
  rfid = new RFID(13, 12);
  servo.attach(17);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  buzzer.beep();
  servo.write(0);
  digitalWrite(LED_BUILTIN, LOW);
}

// Main Loop
void loop() {
  char tag[9];

  if ((closeTime != 0) && (millis() > closeTime)) {
    servo.write(0);
    closeTime = 0;
  }
  if (button.released()) {
    enroll = true;
  }
  if (rfid->read(tag)) {
    if (enroll) {
      if (isAuthorized(tag)) {
        Serial.print ("Tag ");
        Serial.print (tag);
        Serial.println (" already authorized");
      } else if (nAuthorized == MAX_TAGS) {
        Serial.println ("Authorized tags list is full");
      } else {
        digitalWrite(LED_BUILTIN, HIGH);
        strcpy (autorized[nAuthorized++], tag);
        Serial.print ("Tag ");
        Serial.print (tag);
        Serial.println (" authorized");
        delay(300);
        digitalWrite(LED_BUILTIN, LOW);
      }
      enroll = false;
    }
    else if (isAuthorized(tag)) {
        Serial.print ("Tag ");
        Serial.print (tag);
        Serial.println (" authorized");
        servo.write(180); // "open door"
        closeTime = millis()+3000;
    } else {
        Serial.print ("Tag ");
        Serial.print (tag);
        Serial.println (" NOT authorized");
        buzzer.beep();
    }
  }
}

// Check if tag is in authorized list
bool isAuthorized(char * tag) {
  for (int i = 0; i < nAuthorized; i++) {
    if (strcmp(autorized[i], tag) == 0) {
      return true;
    }
  }
  return false;
}
