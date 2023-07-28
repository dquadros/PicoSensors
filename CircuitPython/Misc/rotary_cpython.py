# Rotary Encoder Example
import rotaryio
import board

encoder = rotaryio.IncrementalEncoder(board.GP16, board.GP17)
last_position = encoder.position
val = 50
print(val)
while True:
    position = encoder.position
    if position != last_position:
        while position > last_position:
            last_position = last_position+1
            if val < 100:
                val = val+1
                print(val)
        while position < last_position:
            last_position = last_position-1
            if val > 0:
                val = val-1
                print(val)
