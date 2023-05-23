#include "DHT.h"

// DHT sensors connections (-1 if not used)
#define PIN_DHT11 16
#define PIN_DHT22 17

#if PIN_DHT11 != -1
DHT dht11(PIN_DHT11, DHT11);
#endif

#if PIN_DHT22 != -1
DHT dht22(PIN_DHT22, DHT22);
#endif

// Initialization
void setup() {
  Serial.begin(115200);
  #if PIN_DHT11 != -1
  dht11.begin();
  #endif
  #if PIN_DHT22 != -1
  dht22.begin();
  #endif
}

// Main Loop
void loop() {
  #if PIN_DHT11 != -1
  Serial.print("DHT11  Humidity: ");
  Serial.print(dht11.readHumidity(), 1);
  Serial.print("%, Temperature: ");
  Serial.print(dht11.readTemperature(), 1);
  Serial.println("C");
  #endif

  #if PIN_DHT22 != -1
  Serial.print("DHT22  Humidity: ");
  Serial.print(dht22.readHumidity(), 1);
  Serial.print("%, Temperature: ");
  Serial.print(dht22.readTemperature(), 1);
  Serial.println("C");
  #endif

  delay(3000);
}
