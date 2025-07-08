#include <Keyboard.h>
#include <ConsumerKeyboard.h>

#define MATRIX_ROWS 4
#define MATRIX_COLS 11

const int colPins[MATRIX_COLS] = {14, 16, 10, 9, 8, 7, 6, 5, 21, 20, 19};
const int rowPins[MATRIX_ROWS] = {18, 15, 4, 3};

const uint8_t keymap[MATRIX_ROWS][MATRIX_COLS] = {
  {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', KEY_BACKSPACE},
  {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', 'M'},
  {KEY_LEFT_SHIFT, 'z', 'x', 'c', 'v', 'b', 'n', 'm', '\'', '/', KEY_RETURN},
  {KEY_TAB, KEY_ESC, KEY_LEFT_CTRL, KEY_LEFT_ALT, ',', ' ', ' ', 0, '.', KEY_LEFT_GUI, 0}
};

bool currentState[MATRIX_ROWS][MATRIX_COLS] = {0};
bool previousState[MATRIX_ROWS][MATRIX_COLS] = {0};

#define ULTRA_LOW_LATENCY
#define NO_DEBOUNCE

#ifdef ULTRA_LOW_LATENCY
  #define SCAN_DELAY_US 100
#else
  #define SCAN_DELAY_US 1000
#endif

#ifndef NO_DEBOUNCE
  #define DEBOUNCE_TIME 3
#endif

bool mode2 = false;

void setup() {
  for (int i = 0; i < MATRIX_ROWS; i++) {
    pinMode(rowPins[i], OUTPUT);
    digitalWrite(rowPins[i], HIGH);
  }
  for (int i = 0; i < MATRIX_COLS; i++) {
    pinMode(colPins[i], INPUT_PULLUP);
  }
  Keyboard.begin();
  delay(500);
}

void loop() {
  scanMatrix();
  delayMicroseconds(SCAN_DELAY_US);
}

void scanMatrix() {
  for (int row = 0; row < MATRIX_ROWS; row++) {
    digitalWrite(rowPins[row], LOW);
    delayMicroseconds(5);
    for (int col = 0; col < MATRIX_COLS; col++) {
      bool keyState = !digitalRead(colPins[col]);
      #ifdef NO_DEBOUNCE
        if (keyState != currentState[row][col]) {
          currentState[row][col] = keyState;
          processKey(row, col, keyState);
        }
      #else
        if (keyState != previousState[row][col]) {
          static unsigned long lastTime;
          lastTime = millis();
          if ((millis() - lastTime) > DEBOUNCE_TIME) {
            if (keyState != currentState[row][col]) {
              currentState[row][col] = keyState;
              processKey(row, col, keyState);
            }
          }
        }
      #endif
      previousState[row][col] = keyState;
    }
    digitalWrite(rowPins[row], HIGH);
  }
}

inline void processKey(int row, int col, bool pressed) {
  uint8_t key = keymap[row][col];
  if (row == 1 && col == 10 && pressed) {
    mode2 = !mode2;
    return;
  }
  if (!mode2) {
    if (key != 0) {
      if (pressed) {
        Keyboard.press(key);
      } else {
        Keyboard.release(key);
      }
    }
  } else {
    if (pressed) {
      if (row == 0 && col >= 0 && col <= 8) {
        Keyboard.write('1' + col);
      }
      else if (row == 1 && col == 0) { // A - giảm sáng
        ConsumerKeyboard.press(KEY_BRIGHTNESS_DECREMENT);
        ConsumerKeyboard.release();
      }
      else if (row == 1 && col == 1) { // S - tăng sáng
        ConsumerKeyboard.press(KEY_BRIGHTNESS_INCREMENT);
        ConsumerKeyboard.release();
      }
      else if (row == 1 && col == 2) { // D - giảm âm lượng
        ConsumerKeyboard.press(KEY_VOLUME_DECREMENT);
        ConsumerKeyboard.release();
      }
      else if (row == 1 && col == 3) { // F - tăng âm lượng
        ConsumerKeyboard.press(KEY_VOLUME_INCREMENT);
        ConsumerKeyboard.release();
      }
      else if (row == 1 && col == 4) { // G
        Keyboard.write('-');
      }
      else if (row == 1 && col == 5) { // H
        Keyboard.write('=');
      }
    }
  }
} 