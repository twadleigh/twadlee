#include "Matrix.h"

#include <cstdint>

#include <Arduino.h>

#include "Logging.h"

// extra defines for key codes to normalize them all to 5 chars
#define MLSFT MODIFIERKEY_SHIFT
#define MLCTL MODIFIERKEY_CTRL
#define MLALT MODIFIERKEY_ALT
#define MLGUI MODIFIERKEY_GUI
#define MRSFT MODIFIERKEY_RIGHT_SHIFT
#define MRALT MODIFIERKEY_RIGHT_ALT
#define MRGUI MODIFIERKEY_RIGHT_GUI
#define MRCTL MODIFIERKEY_RIGHT_CTRL
#define K_TLD KEY_TILDE
#define K_MNS KEY_MINUS
#define K_EQL KEY_EQUAL
#define K_PUP KEY_PAGE_UP
#define K_PDN KEY_PAGE_DOWN
#define K_HOM KEY_HOME
#define K_END KEY_END
#define K_TAB KEY_TAB
#define K_LBR KEY_LEFT_BRACE
#define K_RBR KEY_RIGHT_BRACE
#define K_BSL KEY_BACKSLASH
#define K_ESC KEY_ESC
#define K_SMC KEY_SEMICOLON
#define K_QUT KEY_QUOTE
#define K_ENT KEY_ENTER
#define K_CMA KEY_COMMA
#define K_PRD KEY_PERIOD
#define K_SLH KEY_SLASH
#define K_BSP KEY_BACKSPACE
#define K_SPC KEY_SPACE
#define K_MNU KEY_MENU
#define K__UP KEY_UP
#define K__DN KEY_DOWN
#define K_LFT KEY_LEFT
#define K_RGT KEY_RIGHT

// special keys
#define XXXXX 0x0000  // no key - ignore event
#define XXX XXXXX
#define _____ 0xFF00  // transparent - look in lower layer for code
#define K_MS1 0xFF01  // mouse 1
#define K_MS2 0xFF02  // mouse 2
#define K_FNC 0xFF03  // raise to 2nd layer

#define LAYOUT( \
    k00, k01, k02, k03, k04, k05, k06, k07, k50, k51, k52, k53, k54, k55, k56, k57, \
      k10, k12, k13, k14, k15, k16, k17, k60, k61, k62, k63, k64, k65, k66, k67, \
        k20, k22, k23, k24, k25, k27, /**/ k70, k71, k72, k73, k74, k75, k76, \
          k30, k32, k33, k34, k35, k36, k37, k80, k81, k82, k84, k85, k86, \
            k40, k41, k42, k44, k46, k47, k90, k91, k94, k95, k96, k97 \
) \
{ \
    {k00, k01, k02, k03, k04, k05, k06, k07}, \
    {k10, XXX, k12, k13, k14, k15, k16, k17}, \
    {k20, XXX, k22, k23, k24, k25, XXX, k27}, \
    {k30, XXX, k32, k33, k34, k35, k36, k37}, \
    {k40, k41, k42, XXX, k44, XXX, k46, k47}, \
    {k50, k51, k52, k53, k54, k55, k56, k57}, \
    {k60, k61, k62, k63, k64, k65, k66, k67}, \
    {k70, k71, k72, k73, k74, k75, k76, XXX}, \
    {k80, k81, k82, XXX, k84, k85, k86, XXX}, \
    {k90, k91, XXX, XXX, k94, k95, k96, k97} \
}

namespace Matrix {
  const uint8_t kNumLayers = 2;
  const uint8_t kNumCols = 8;
  static const uint8_t kColPins[] = {7, 6, 8, 26, 24, 4, 3, 2};
  const uint8_t kNumRows = 10;
  static const uint8_t kRowPins[] = {9, 10, 11, 12, 14, 27, 31, 28, 29, 30};

  const uint16_t kLayout[kNumLayers][kNumRows][kNumCols] = {
    LAYOUT(
      K_TLD, KEY_1, KEY_2, KEY_3, KEY_4, KEY_5, KEY_6, MLGUI, KEY_7, KEY_8, KEY_9, KEY_0, K_MNS, K_EQL, K_PUP, K_PDN,
         K_TAB, KEY_Q, KEY_W, KEY_E, KEY_R, KEY_T, MLALT, KEY_Y, KEY_U, KEY_I, KEY_O, KEY_P, K_LBR, K_RBR, K_BSL,
            K_ESC, KEY_A, KEY_S, KEY_D, KEY_F, KEY_G, /****/ KEY_H, KEY_J, KEY_K, KEY_L, K_SMC, K_QUT, K_ENT,
               MLSFT, KEY_Z, KEY_X, KEY_C, KEY_V, KEY_B, MLCTL, KEY_N, KEY_M, K_CMA, K_PRD, K_SLH, MRSFT,
                  MLCTL, MLGUI, MLALT, K_BSP, K_MS1, K_FNC, K_MS2, K_SPC, MRALT, K_MNU, MRGUI, MRCTL),
    LAYOUT(
      _____, _____, _____, _____, _____, _____, _____, _____, _____, _____, _____, _____, _____, _____, _____, _____,
         _____, _____, _____, _____, _____, _____, _____, K_HOM, K_PDN, K_PUP, K_END, _____, _____, _____, _____,
            _____, _____, _____, _____, _____, _____, /****/ K_LFT, K__DN, K__UP, K_RGT, _____, _____, _____,
               _____, _____, _____, _____, _____, _____, _____, _____, _____, _____, _____, _____, _____,
                  _____, _____, _____, _____, _____, _____, _____, _____, _____, _____, _____, _____)};

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

    // process some special key codes
    if (XXX == key_code) return;  // null event

    // transparency: select key from below
    while(_____ == key_code && layer > 0) {
      --layer;
      key_code = kLayout[layer][row][col];
    }

    if (_____ == key_code) {
      ERROR("can't have transparency in bottom layer");
      return;
    } else {
      gKeyStateMatrix[row][col].SetLastCode(key_code);
      switch(key_code >> 8) {
        case 0xFF:  // special key
          switch(key_code) {
            case K_MS1:
              Mouse.press(1);
              break;
            case K_MS2:
              Mouse.press(2);
              break;
            case K_FNC:
              gCurrentLayer = 1;
              //Mouse.press(4);
              break;
            default:
              ERROR("Unrecognized special keycode in key press event");
          }
        default:
          Keyboard.press(key_code);
          return;
      }
    }
  }

  void ProcessKeyReleaseEvent(uint8_t row, uint8_t col) {
    INFO("key release event: (%u, %u)", row, col);
    uint16_t key_code = gKeyStateMatrix[row][col].ClearLastCode();
    switch(key_code >> 8) {
      case 0xFF:  // special key
        switch(key_code) {
          case K_MS1:
            Mouse.release(1);
          case K_MS2:
            Mouse.release(2);
          case K_FNC:
            gCurrentLayer = 0;
            //Mouse.release(4);
          default:
            ERROR("Unrecognized special keycode in key release event");
        }
      default:
        Keyboard.release(key_code);
        return;
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
