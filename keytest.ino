#include <Keyboard.h>

// Định nghĩa kích thước ma trận phím
#define MATRIX_ROWS 4
#define MATRIX_COLS 11

// Định nghĩa các chân Arduino Pro Micro
const int colPins[MATRIX_COLS] = {14, 16, 10, 9, 8, 7, 6, 5, 21, 20, 19};
const int rowPins[MATRIX_ROWS] = {18, 15, 4, 3};

// Bố cục phím - sử dụng mã ASCII cho các phím chữ/số
const uint8_t keymap[MATRIX_ROWS][MATRIX_COLS] = {
  {'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', KEY_BACKSPACE},
  {'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', 0},
  {KEY_LEFT_SHIFT, 'z', 'x', 'c', 'v', 'b', 'n', 'm', '\'', '/', KEY_RETURN},
  {KEY_TAB, KEY_ESC, KEY_LEFT_CTRL, KEY_LEFT_ALT, ',', ' ', ' ', 0, '.', KEY_LEFT_GUI, 0}
};

// Trạng thái phím hiện tại và trước đó
bool currentState[MATRIX_ROWS][MATRIX_COLS] = {0};
bool previousState[MATRIX_ROWS][MATRIX_COLS] = {0};

// Cấu hình cho chế độ gaming
#define GAMING_MODE_ENABLED true  // Luôn bật chế độ gaming
#define ULTRA_LOW_LATENCY    // Định nghĩa mode độ trễ cực thấp
#define NO_DEBOUNCE         // Bỏ qua debounce để tăng tốc (thận trọng với một số switch)

#ifdef ULTRA_LOW_LATENCY
  #define SCAN_DELAY_US 100  // Microseconds - 0.1ms giữa các lần quét, cực nhanh
#else
  #define SCAN_DELAY_US 1000 // 1ms - Gaming tốc độ thường
#endif

#ifndef NO_DEBOUNCE
  #define DEBOUNCE_TIME 3    // Giảm thời gian debounce xuống 3ms
#endif

void setup() {
  // Tắt Serial để giảm độ trễ
  
  // Thiết lập chế độ fast pinMode
  for (int i = 0; i < MATRIX_ROWS; i++) {
    pinMode(rowPins[i], OUTPUT);
    // Sử dụng direct port manipulation nếu có thể
    digitalWrite(rowPins[i], HIGH);
  }
  
  for (int i = 0; i < MATRIX_COLS; i++) {
    pinMode(colPins[i], INPUT_PULLUP);
  }
  
  Keyboard.begin();
  
  // Giảm thời gian khởi động
  delay(500);
}

void loop() {
  scanMatrix();
  
  // Sử dụng delayMicroseconds thay vì delay để có độ trễ thấp
  delayMicroseconds(SCAN_DELAY_US);
}

void scanMatrix() {
  for (int row = 0; row < MATRIX_ROWS; row++) {
    digitalWrite(rowPins[row], LOW);
    
    // Giảm thời gian ổn định tín hiệu
    delayMicroseconds(5);
    
    for (int col = 0; col < MATRIX_COLS; col++) {
      bool keyState = !digitalRead(colPins[col]);
      
      #ifndef NO_DEBOUNCE
        // Chỉ sử dụng debounce khi không ở chế độ NO_DEBOUNCE
        if (keyState != previousState[row][col]) {
          // Sử dụng biến static để tối ưu bộ nhớ
          static unsigned long lastTime;
          lastTime = millis();
          
          if ((millis() - lastTime) > DEBOUNCE_TIME) {
            if (keyState != currentState[row][col]) {
              currentState[row][col] = keyState;
              processKey(row, col, keyState);
            }
          }
        }
      #else
        // Chế độ không có debounce - phản hồi tức thì
        if (keyState != currentState[row][col]) {
          currentState[row][col] = keyState;
          processKey(row, col, keyState);
        }
      #endif
      
      previousState[row][col] = keyState;
    }
    
    digitalWrite(rowPins[row], HIGH);
  }
}

// Tách xử lý phím thành hàm riêng để tối ưu
inline void processKey(int row, int col, bool pressed) {
  uint8_t key = keymap[row][col];
  
  if (key != 0) {
    if (pressed) {
      Keyboard.press(key);
    } else {
      Keyboard.release(key);
    }
  }
  
  // Xử lý phím chuyển chế độ nếu cần
  if (row == 1 && col == 10 && pressed) {
    // Có thể dùng để chuyển đổi giữa các profile gaming khác nhau
  }
}