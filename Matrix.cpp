#include "Matrix.h"

#include <cstdint>

#include <Arduino.h>

#include "KeyDefs.h"
#include "Logging.h"

namespace Matrix {
  const uint8_t kNumLayers = 2;
  const uint8_t kNumCols = 8;
  static const uint8_t kColPins[] = {7, 6, 8, 26, 24, 4, 3, 2};
  const uint8_t kNumRows = 10;
  static const uint8_t kRowPins[] = {9, 10, 11, 12, 14, 27, 31, 28, 29, 30};

  const uint16_t kLayout[kNumLayers][kNumRows][kNumCols] = {
    LAYOUT(
      TILDE, __1__, __2__, __3__, __4__, __5__, __6__, L_GUI, __7__, __8__, __9__, __0__, MINUS, EQUAL, WAKE_, SLEEP,
         _TAB_, __Q__, __W__, __E__, __R__, __T__, L_ALT, __Y__, __U__, __I__, __O__, __P__, L_BRC, R_BRC, BK_SL,
            _ESC_, __A__, __S__, __D__, __F__, __G__, /****/ __H__, __J__, __K__, __L__, SEMIC, QUOTE, ENTER,
               L_SFT, __Z__, __X__, __C__, __V__, __B__, L_CTL, __N__, __M__, COMMA, PRIOD, SLASH, R_SFT,
                  L_CTL, L_GUI, L_ALT, BK_SP, MBTNL, ML(1), MBTNR, SPACE, R_ALT, MENU_, R_GUI, R_CTL),
    LAYOUT(
      NUMLK, _F01_, _F02_, _F03_, _F04_, _F05_, _F06_, _____, _F07_, _F08_, _F09_, _F10_, _F11_, _F12_, PRSCR, _OFF_,
         SCRLK, _____, _____, _____, NEXT_, VOLUP, _____, HOME_, PG_DN, PG_UP, _END_, _____, _____, _____, _____,
            CAPLK, _____, _____, _____, PLAY_, MUTE_, /****/ LEFT_, DOWN_, _UP__, RIGHT, _____, _____, _____,
               PAUSE, _____, _____, _____, PREV_, VOLDN, _____, _____, _____, _____, _____, _____, _____,
                  _____, _____, _____, _DEL_, _____, XXXXX, _____, _INS_, _____, _____, _____, _____)};

  static const uint16_t kDebouncingTimeMs = 5;
  struct KeyState {
    uint8_t Pin;
    uint16_t LastCode;
    uint64_t TimeDoneDebouncingMs;

    KeyState() : Pin(0), LastCode(0), TimeDoneDebouncingMs(0) {}

    void AttachToPin(uint8_t pin) {
      Pin = pin;
    }

    void Reset() {
      TimeDoneDebouncingMs = 0;
    }

    void SetLastCode(uint16_t key_code) {
      if (0 != LastCode) {
        ERROR("Non-null last key code for key press event");
      }
      LastCode = key_code;
    }

    uint16_t ClearLastCode() {
      uint16_t key_code = LastCode;
      LastCode = 0;
      return key_code;
    }

    // 0: no change, 1: press, 2: release
    uint8_t Update() {
      if(LOW == digitalRead(Pin)) {
        if (0 == TimeDoneDebouncingMs) {
          TimeDoneDebouncingMs = millis() + kDebouncingTimeMs;
          return 1;
        }
      } else {
        if (TimeDoneDebouncingMs > 0 && millis() >= TimeDoneDebouncingMs) {
          Reset();
          return 2;
        }
      }
      return 0;
    }
  };

  uint8_t gCurrentLayer = 0;
  static KeyState gKeyStateMatrix[kNumRows][kNumCols];
  static void ProcessKeyPressEvent(uint8_t row, uint8_t col, uint8_t layer = gCurrentLayer) {
    INFO("key press event: (%u, %u)", row, col);
    uint16_t key_code = kLayout[layer][row][col];
    uint16_t key_mask = key_code >> 8;
    if (key_mask >= MIN_RESERVED_MASK) {
      // process a regular key event
      gKeyStateMatrix[row][col].SetLastCode(key_code);
      Keyboard.press(key_code);
    } else {
      // process special key event
      switch (key_mask) {
        case MOUSE_BUTTON_MASK:
          gKeyStateMatrix[row][col].SetLastCode(key_code);
          Mouse.press(key_code & 0xFF);
          return;
        case MODAL_LAYER_MASK:
          gKeyStateMatrix[row][col].SetLastCode(ML(gCurrentLayer));
          gCurrentLayer = (key_code & 0xFF);
          return;
        case TRANSPARENCY_MASK:
          if (0 == layer) {
            ERROR("can't have transparency in layer zero");
          } else {
            ProcessKeyPressEvent(row, col, layer-1);
          }
          return;
        case NULL_MASK:
          return;
        default:
          ERROR("unexpected key mask %u", key_mask);
          return;
      }
    }
  }

  void ProcessKeyReleaseEvent(uint8_t row, uint8_t col) {
    INFO("key release event: (%u, %u)", row, col);
    uint16_t key_code = gKeyStateMatrix[row][col].ClearLastCode();
    uint16_t key_mask = key_code >> 8;
    if (key_mask >= MIN_RESERVED_MASK) {
      Keyboard.release(key_code);
    } else {
      switch (key_mask) {
        case MOUSE_BUTTON_MASK:
          Mouse.release(key_code & 0xFF);
          return;
        case MODAL_LAYER_MASK:
          gCurrentLayer = (key_code & 0xFF);
          return;
        default:
          ERROR("unexpected key mask %u", key_mask);
          return;
      }
    }
  }

  void Init() {
    // initialize the pin states
    for (uint8_t i = 0; i < kNumRows; ++i) {
      pinMode(kRowPins[i], INPUT_PULLUP);
    }
    for (uint8_t i = 0; i < kNumCols; ++i) {
      pinMode(kColPins[i], INPUT_PULLUP);
    }
    // initialize the debouncer matrix
    for (uint8_t r = 0; r < kNumRows; ++r) {
      for (uint8_t c = 0; c < kNumCols; ++c) {
        gKeyStateMatrix[r][c].AttachToPin(kColPins[c]);
      }
    }
  }

  void Scan() {
    for (uint8_t r = 0; r < kNumRows; ++r) {
      KeyState* keystate_row = gKeyStateMatrix[r];
      // set the row pin low
      uint8_t pin = kRowPins[r];
      pinMode(pin, OUTPUT);
      digitalWrite(pin, LOW);
      delayMicroseconds(1);  // allow pin to settle

      // scan columns
      for (uint8_t c = 0; c < kNumCols; ++c) {
        uint8_t result = keystate_row[c].Update();
        if (0 == result) continue;
        if (1 == result) {
          ProcessKeyPressEvent(r, c);
        } else if (2 == result) {
          ProcessKeyReleaseEvent(r, c);
        } else {
          ERROR("unexpected result from debouncer: %u", result);
        }
      }

      // restore the row pin state to input
      pinMode(pin, INPUT_PULLUP);
    }
  }
}  // namespace Matrix
