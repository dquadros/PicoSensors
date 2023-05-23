# DHT11/DHT22 Example

import time
import adafruit_dht
import board

# Connections, -1 = not used
pinDHT11 = board.GP16
pinDHT22 = board.GP17

if pinDHT11 != -1:
    dht11 = adafruit_dht.DHT11(pinDHT11)
    
if pinDHT22 != -1:
    dht22 = adafruit_dht.DHT22(pinDHT22)
 
while True:
    if pinDHT11 != -1:
        print("DHT11 Humidity: %.1f%%, Temperature: %.1fC" % 
          (dht11.humidity, dht11.temperature))
    if pinDHT22 != -1:
        print("DHT22 Humidity: %.1f%%, Temperature: %.1fC" % 
          (dht22.humidity, dht22.temperature))
    time.sleep(3)

