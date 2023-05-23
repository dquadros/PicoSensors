// DS18B20 Temperature Sensor Example
// Requires the OneWireNg library
// Adapted from DallasTemperature example

#include "OneWireNg_CurrentPlatform.h"
#include "drivers/DSTherm.h"
#include "utils/Placeholder.h"

// One Wire bus is connected to this pin
# define OW_PIN         16

static Placeholder<OneWireNg_CurrentPlatform> ow;

// Initialization
void setup() {
  // Init the serial
  Serial.begin(115200);

  // Instanciate the onewire bus
  new (&ow) OneWireNg_CurrentPlatform(OW_PIN, false);
}

// Main loop
void loop() {
  DSTherm drv(ow);

  // Start convertion in all sensors
  drv.convertTempAll(DSTherm::MAX_CONV_TIME, false);

  // Read temperature from the sensors
  Placeholder<DSTherm::Scratchpad> scrpd;
  for (const auto& id: *ow) {
      if (printId(id)) {
        if (drv.readScratchpad(id, scrpd) == OneWireNg::EC_SUCCESS) {
          const uint8_t *scrpd_raw = ((const DSTherm::Scratchpad&) scrpd).getRaw();
          long temp = ((const DSTherm::Scratchpad&) scrpd).getTemp();
          Serial.print(" Temp:");
          if (temp < 0) {
              temp = -temp;
              Serial.print('-');
          }
          Serial.print(temp / 1000);
          Serial.print('.');
          Serial.print(temp % 1000);
          Serial.print(" C");
        }
        Serial.println();
      }
  }
  Serial.println();
  delay(250);
}

// Print the device address and family name
// returns false if not supported
static bool printId(const OneWireNg::Id& id)
{
    const char *name = DSTherm::getFamilyName(id);

    Serial.print(id[0], HEX);
    for (size_t i = 1; i < sizeof(OneWireNg::Id); i++) {
        Serial.print(':');
        Serial.print(id[i], HEX);
    }
    if (name) {
        Serial.print(" -> ");
        Serial.print(name);
    }

    return (name != NULL);
}

