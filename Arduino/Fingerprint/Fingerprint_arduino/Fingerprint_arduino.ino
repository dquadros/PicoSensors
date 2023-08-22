// Fingerprint Sensor Example

#include <Adafruit_Fingerprint.h>

#define mySerial Serial1
Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// Conexões do LED RGB
#define LED_R_PIN     13
#define LED_G_PIN     14
#define LED_B_PIN     15

// Initialization
void setup() {
  // Start communication with the PC
  while (!Serial)
    ;
  Serial.begin(115200);
  delay(100);
  Serial.println("\n\nFingerprint Sensor Example\n");

  // Init LED
  pinMode (LED_R_PIN, OUTPUT); digitalWrite(LED_R_PIN, LOW);
  pinMode (LED_G_PIN, OUTPUT); digitalWrite(LED_G_PIN, LOW);
  pinMode (LED_B_PIN, OUTPUT); digitalWrite(LED_B_PIN, LOW);

  // Init sensor
  Serial1.setRX(17);
  Serial1.setTX(16);
  finger.begin(57600);

  // Shows sensor capacity
  Serial.println("Checking sensor capacity");
  finger.getParameters();
  Serial.print("Sensor can store "); 
  Serial.print(finger.capacity);
  Serial.println(" fingerprints"); 

  // Clear stored fingerprints
  finger.getTemplateCount();
  int count = finger.templateCount;
  if (count > 0) {
    Serial.print("Erasing ");
    Serial.print(count);
    Serial.println(" fingerprints");
    if (finger.emptyDatabase() == FINGERPRINT_OK) {
      Serial.println ("Success");
    }
  }
}

// Main Loop
void loop() {
  Serial.println();
  captureFeature(1);
  Serial.println("Searching...");
  uint8_t ret = finger.fingerSearch();
  if (ret == FINGERPRINT_NOTFOUND) {
    Serial.println("Unknown, let\'s enroll");
    enroll();
  } else if (ret == FINGERPRINT_OK) {
    Serial.print("Fingerprint ");
    Serial.print(finger.fingerID);
    Serial.println(" identified");
  } else {
    Serial.print ("Error ");
    Serial.println (ret);
  }
}

// Read fingerprint and create template
void captureFeature(uint8_t numbuf) {
  while (true) {
    digitalWrite(LED_B_PIN, HIGH);
    Serial.println ("Place finger on sensor");
    while (finger.getImage() != FINGERPRINT_OK) {
      delay(10);
    }
    digitalWrite(LED_B_PIN, LOW);
    Serial.println("Image captured");

    bool ok = finger.image2Tz(numbuf) == FINGERPRINT_OK;
    Serial.println ("Remove finger from sensor");
    delay(2000);
    if (ok) {
      Serial.println ("Feature created");
      return;
    }
    digitalWrite(LED_R_PIN, HIGH);
    Serial.println ("Bad image, try again");
    delay(1000);
    digitalWrite(LED_R_PIN, LOW);
  }
}

// Enroll fingerprint
void enroll() {
  bool first = true;
  while (true) {
    if (!first) {
      captureFeature(1);
    }
    captureFeature(2);
    if (finger.createModel() != FINGERPRINT_OK) {
      digitalWrite(LED_R_PIN, HIGH);
      Serial.println ("Images do not match, try again");
      delay(1000);
      digitalWrite(LED_R_PIN, LOW);
      continue;
    }
    finger.getTemplateCount();
    int pos = finger.templateCount;
    int ret = finger.storeModel(pos);
    if (ret == FINGERPRINT_OK) {
      digitalWrite(LED_G_PIN, HIGH);
      Serial.print ("Fingerprint stored at ");
      Serial.println (pos);
      delay(1000);
      digitalWrite(LED_G_PIN, LOW);
      return;
    }
    digitalWrite(LED_R_PIN, HIGH);
    Serial.print ("Error ");
    Serial.print (ret);
    Serial.println (" while storing fingerprint");
    delay(1000);
    digitalWrite(LED_R_PIN, LOW);
  }
}
