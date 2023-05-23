# Phototransistor example

import analogio
import digitalio
import board

# Phototransistor at ADC0
sensor = analogio.AnalogIn(board.A0)

val_ant = 0

# Main loop
while True:
    val = sensor.value
    dif = abs(val - val_ant)
    if dif > 5000:
        print(val)
        val_ant = val

