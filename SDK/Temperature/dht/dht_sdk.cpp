/**
 * @file dht_sdk.c
 * @author Daniel Quadros
 * @brief DHT Temperature Sensor Example
 * @version 1.0
 * @date 2023-05-20
 * 
 * @copyright Copyright (c) 2022, Daniel Quadros
 * 
 */

#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"

// Our PIO program:
#include "dht_sdk.pio.h"

// DHT sensors connections (-1 if not used)
#define PIN_DHT11 16
#define PIN_DHT22 17

// PIO
static PIO pio = pio0;
static uint offset = 0xFFFF;

// Get milliseconds from start
static inline uint32_t board_millis(void)
{
	return to_ms_since_boot(get_absolute_time());
}

// Class to access DHT sensors
class DHT {
    public:
        typedef enum { DHT11, DHT22 } Model;

    private:
        uint dataPin;
        Model model;
        uint sm;
        uint32_t lastreading;
        uint8_t data [5];

        // get data from sensor
        bool read() {
            // Init and start the state machine
            dht_program_init(pio, sm, offset, dataPin);
            // Start a reading
            pio_sm_put (pio, sm, (model == DHT11) ? 969 : 54);
            // Read 5 bytes
            for (int i = 0; i < 5; i++) {
                data[i] = (uint8_t) pio_sm_get_blocking (pio, sm);
            }
            // Stop the state machine
            pio_sm_set_enabled (pio, sm, false);
            uint32_t total = 0;
            for (int i = 0; i < 4; i++) {
                total += data[i];
            }
            if (data[4] == (total & 0xFF)) {
                lastreading = board_millis();
                return true;
            }
            return false;
        }

        // use cached data or read from sensor
        void getData() {
            // make sure we have some data
            while (lastreading == 0) {
                if (!read()) {
                    sleep_ms(2000);
                }
            }
            // use cache if less than 2 seconds
            uint32_t now = board_millis();
            if (lastreading > now) { // count wraped
                lastreading = now;
            }
            if ((lastreading+2000) < now) {
                read();
            }
        }


    public:

        // Constructor
        DHT (uint pin, Model model) {
            this->dataPin = pin;
            this->model = model;
            this->sm = pio_claim_unused_sm(pio, true);
            this->lastreading = 0;
            if (offset == 0xFFFF) {
                offset = pio_add_program(pio, &dht_program);
            }
        }

        // get humidity
        float humidity() {
            getData();
            if (model == DHT11) {
                return 0.1*data[1] + data[0];
            } else {
                return 0.1*((data[0] << 8) + data[1]);
            }
        }

        // get temperature
        float temperature() {
            getData();
            if (model == DHT11) {
                return 0.1*data[3] + data[2];
            } else {
                float s = (data[2] & 0x80) ? -0.1 : 0.1;
                return s*(((data[2] & 0x7f) << 8) + data[3]);
            }
        }
};


int main() {
    // Init stdio
    stdio_init_all();
    #ifdef LIB_PICO_STDIO_USB
    while (!stdio_usb_connected()) {
        sleep_ms(100);
    }
    #endif

    printf("\nDHT11/DHT22 Example\n");

    DHT *dht11 = (PIN_DHT11 == -1)? NULL : new DHT(PIN_DHT11, DHT::DHT11);
    DHT *dht22 = (PIN_DHT22 == -1)? NULL : new DHT(PIN_DHT22, DHT::DHT22);

    while (true) {
        if (PIN_DHT11 != -1) {
            printf("DHT11 Humidity: %.1f%%, Temperature: %.1fC\n",
                dht11->humidity(), dht11->temperature());
        }
        if (PIN_DHT22 != -1) {
            printf("DHT22 Humidity: %.1f%%, Temperature: %.1fC\n",
                dht22->humidity(), dht22->temperature());
        }
        sleep_ms(3000);
    }
}
