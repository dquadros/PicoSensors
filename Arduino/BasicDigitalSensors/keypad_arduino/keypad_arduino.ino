// Keypad Example

// Keypad pins
#define NROWS     4
#define NCOLUMNS  3
#define ROW_1     0
#define ROW_2     1
#define ROW_3     2
#define ROW_4     3
#define COL_1     4
#define COL_2     5
#define COL_3     6

// Debounce
#define DEBOUNCE 5

// Keypad control variables
int rows[] = { ROW_1, ROW_2, ROW_3, ROW_4 };
int columns[] = { COL_1, COL_2, COL_3 };
int curRow;
bool previous [NROWS][NCOLUMNS];
bool validated [NROWS][NCOLUMNS];
int count [NROWS][NCOLUMNS];

// Key codes
char codes[NROWS][NCOLUMNS] = { 
 { '1', '2', '3' },
 { '4', '5', '6' },
 { '7', '8', '9' },
 { '*', '0', '#' }
};

// Keys queue
#define MAX_KEYS 16
char keys[MAX_KEYS+1];
int inkey, outkey;

// Check for pressed keys in current row
void checkRow() {
  // set current row to LOW
  int pin = rows[curRow];
  pinMode (pin, OUTPUT);
  digitalWrite(pin, LOW);

  // read columns and check for changes
  for (int col = 0; col < NCOLUMNS; col++) {
    bool read = digitalRead (columns[col]) == LOW;
    if (read == previous[curRow][col]) {
      if (count[curRow][col] != 0) {
        if (--count[curRow][col] == 0) {
          // reading validated
          if (read != validated[curRow][col]) {
            validated[curRow][col] = read;
            if (read) {
              // keypress detected
              int aux = inkey+1;
              if (aux > MAX_KEYS) {
                aux = 0;
              }
              if (aux != outkey) {
                keys[inkey] = codes[curRow][col];
                inkey = aux;
              }
            }
          }
        }
      }
    } else {
      // restart validation
      previous[curRow][col] = read;
      count[curRow][col] = DEBOUNCE;
    }
  }
  // return row to input
  pinMode (pin, INPUT);
  // move to next row
  curRow = (curRow+1) % NROWS;
}

// Read a key from the key queue, returns -1 if queue empty
int getKey(void) {
    int key = -1;
    if (inkey != outkey) {
        key = keys[outkey];
        outkey = (outkey == MAX_KEYS) ? 0 : (outkey + 1);
    }
    return key;
}

// Initializations
void setup() {
  Serial.begin(115200);
  Serial.println("Keypad Example");

  // init keypad controls
  curRow = 0;
  for (int i = 0; i < NROWS; i++) {
    for (int j = 0; j < NCOLUMNS; j++) {
      previous[i][j] = validated[i][j] = false;
      count[i][j] = DEBOUNCE;
    }
  }

  // init keys queue
  inkey = outkey = 0;

  // init keypad pins
  for (int i = 0; i < NROWS; i++) {
    pinMode(rows[i], INPUT);
  }
  for (int i = 0; i < NCOLUMNS; i++) {
    pinMode(columns[i], INPUT_PULLUP);
  }
}

// Main Loop
void loop() {
  checkRow();
  int key = getKey();
  if (key == -1) {
    delay(10);
  } else {
    Serial.print ("Pressed ");
    Serial.println ((char) key);
    Serial.flush ();
  }
}
