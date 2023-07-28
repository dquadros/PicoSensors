// iButton Example
// Requires the OneWireNg library

#include "OneWireNg_CurrentPlatform.h"
#include "utils/Placeholder.h"


// One Wire bus is connected to this pin
# define OW_PIN         16

static Placeholder<OneWireNg_CurrentPlatform> ow;

static OneWireNg::Id last;

// Initialization
void setup() {
  // Init the serial
  while (!Serial) {
    delay(100);
  }
  Serial.begin(115200);

  // Instanciate the onewire bus
  new (&ow) OneWireNg_CurrentPlatform(OW_PIN, false);
  memset(last, 0xFF, sizeof(OneWireNg::Id));
}

// Main loop
void loop() {
  OneWireNg::Id id;

  ow->searchReset();
  if ((ow->search(id) == OneWireNg::EC_SUCCESS) &&
      (id[0] == 0x01) && 
      (memcmp(id, last, sizeof(OneWireNg::Id)) != 0)) {
    Serial.print("ID: ");
    for (size_t i = sizeof(OneWireNg::Id)-2; i > 0; i--) {
        Serial.print(id[i], HEX);
    }
    Serial.println();
    memcpy(last, id, sizeof(OneWireNg::Id));
    delay(2000);
  } else {
    memset(last, 0xFF, sizeof(OneWireNg::Id));
    delay (500);
  }
}
