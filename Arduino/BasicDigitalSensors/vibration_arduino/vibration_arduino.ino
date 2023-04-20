// Vibration Sensor Example

#define SENSOR_PIN 16

#define N_SAMPLES 100

// Samples storage
uint8_t vibr[N_SAMPLES];
int inVibr, outVibr, nVibr;
int count;

// Initializations
void setup() {
  pinMode (LED_BUILTIN, OUTPUT);
  digitalWrite (LED_BUILTIN, LOW);
  pinMode (SENSOR_PIN, INPUT);
  inVibr = outVibr = nVibr = 0;
  count = 0;
}

// Main Loop
void loop() {
  delay(10);
  if (nVibr == N_SAMPLES) {
    // Remove oldest
    count -= vibr[outVibr];
    outVibr = (outVibr+1) % N_SAMPLES;
  } else {
    nVibr++;
  }
  vibr[inVibr] = digitalRead(SENSOR_PIN);
  count += vibr[inVibr];
  inVibr = (inVibr+1) % N_SAMPLES;
  digitalWrite (LED_BUILTIN, (count > 80)? HIGH : LOW);
}
