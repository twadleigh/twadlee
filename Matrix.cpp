#include "Matrix.h"

#include <Arduino.h>

#include "KeyDefs.h"
#include "Leds.h"
#include "Logging.h"

#define LAYOUT( \
    k_0_0, k_0_1, k_0_2, k_0_3, k_0_4, k_0_5, k_0_6, k_0_7, k_0_8, k_0_9, k_0_a, k_0_b, k_0_c, k_0_d, k_0_e, k_0_f, \
      k_1_0, k_1_1, k_1_2, k_1_3, k_1_4, k_1_5, k_1_6, k_1_7, k_1_8, k_1_9, k_1_a, k_1_b, k_1_c, k_1_d, k_1_f, \
        k_2_0, k_2_2, k_2_3, k_2_4, k_2_5, k_2_6, /**/ k_2_8, k_2_9, k_2_a, k_2_b, k_2_c, k_2_d, k_2_e, \
          k_3_1, k_3_2, k_3_3, k_3_4, k_3_5, k_3_6, k_3_7, k_3_8, k_3_9, k_3_a, k_3_b, k_3_c, k_3_e, \
            k_4_0, k_4_1, k_4_3, k_4_4, k_4_6, k_4_7, k_4_8, k_4_9, k_4_b, k_4_c, k_4_e, k_4_f \
) \
{ \
    {k_0_0, k_0_1, k_0_2, k_0_3, k_0_4, k_0_5, k_0_6, k_0_7, k_0_8, k_0_9, k_0_a, k_0_b, k_0_c, k_0_d, k_0_e, k_0_f}, \
    {k_1_0, k_1_1, k_1_2, k_1_3, k_1_4, k_1_5, k_1_6, k_1_7, k_1_8, k_1_9, k_1_a, k_1_b, k_1_c, k_1_d, XXXXX, k_1_f}, \
    {k_2_0, XXXXX, k_2_2, k_2_3, k_2_4, k_2_5, k_2_6, XXXXX, k_2_8, k_2_9, k_2_a, k_2_b, k_2_c, k_2_d, k_2_e, XXXXX}, \
    {XXXXX, k_3_1, k_3_2, k_3_3, k_3_4, k_3_5, k_3_6, k_3_7, k_3_8, k_3_9, k_3_a, k_3_b, k_3_c, XXXXX, k_3_e, XXXXX}, \
    {k_4_0, k_4_1, XXXXX, k_4_3, k_4_4, XXXXX, k_4_6, k_4_7, k_4_8, k_4_9, XXXXX, k_4_b, k_4_c, XXXXX, k_4_e, k_4_f} \
}

namespace Matrix {
  const uint8_t kNumLayers = 2;
  const uint8_t kNumCols = 16;
  static const uint8_t kColPins[] = {29, 30, 31, 32, 33, 15, 16, 17, 2, 1, 0, 28, 27, 26, 25, 24};
  const uint8_t kNumRows = 5;
  static const uint8_t kRowPins[] = {23, 22, 21, 20, 19};

  const uint16_t kLayout[kNumLayers][kNumRows][kNumCols] = {
    LAYOUT(
      TILDE, __1__, __2__, __3__, __4__, __5__, __6__, L_GUI, __7__, __8__, __9__, __0__, MINUS, EQUAL, VOLDN, VOLUP,
         _TAB_, __Q__, __W__, __E__, __R__, __T__, L_ALT, __Y__, __U__, __I__, __O__, __P__, L_BRC, R_BRC, BK_SL,
            _ESC_, __A__, __S__, __D__, __F__, __G__, /****/ __H__, __J__, __K__, __L__, SEMIC, QUOTE, ENTER,
               L_SFT, __Z__, __X__, __C__, __V__, __B__, L_CTL, __N__, __M__, COMMA, PRIOD, SLASH, R_SFT,
                  L_CTL, L_GUI, L_ALT, BK_SP, MBTNL, RAISE, MBTNR, SPACE, R_ALT, _MENU, R_GUI, R_CTL),
    LAYOUT(
      NUMLK, _F01_, _F02_, _F03_, _F04_, _F05_, _F06_, _____, _F07_, _F08_, _F09_, _F10_, _F11_, _F12_, PRSCR, _MUTE,
         SCRLK, _____, _____, _____, _____, _____, _____, _HOME, PG_DN, PG_UP, _END_, _____, _____, _____, _____,
            CAPLK, _____, _____, _____, _____, _____, /****/ _LEFT, _DOWN, _UP__, RIGHT, _____, _____, _____,
               PAUSE, _____, _____, _____, _____, _____, _____, _PLAY, _NEXT, _PREV, _STOP, _____, _____,
                  _WAKE, SLEEP, _OFF_, _DEL_, _____, XXXXX, _____, _INS_, LEDMN, LEDMX, LEDDN, LEDUP)};

  static const uint16_t kDebouncingTimeMs = 10;
  struct KeyState {
    uint8_t Pin;
    uint8_t LastLayer;
    uint64_t TimeDoneDebouncingMs;

    KeyState() : Pin(0), LastLayer(0), TimeDoneDebouncingMs(0) {}

    void AttachToPin(uint8_t pin) {
      Pin = pin;
    }

    void Reset() {
      TimeDoneDebouncingMs = 0;
    }

    void SetLastLayer(uint8_t layer) {
      if (0 != LastLayer) {
        ERROR("Non-null last layer for key press event");
      }
      LastLayer = layer + 1;
    }

    uint8_t ClearLastLayer() {
      uint8_t layer = LastLayer;
      LastLayer = 0;
      return layer;
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

  uint8_t GetCurrentLayer() {
    return gCurrentLayer;
  }

  static KeyState gKeyStateMatrix[kNumRows][kNumCols];
  static void ProcessKeyPressEvent(uint8_t row, uint8_t col, uint8_t layer = gCurrentLayer) {
    INFO("key press event: (%u, %u)", row, col);
    uint16_t key_code = kLayout[layer][row][col];
    if (_____ == key_code) {
      if (0 == layer) {
        ERROR("Can't have transparency in layer 0");
      } else {
        ProcessKeyPressEvent(row, col, layer-1);
      }
    } else {
      gKeyStateMatrix[row][col].SetLastLayer(layer);
      if (key_code >= FIRST_RESERVED_CODE) {
        Keyboard.press(key_code);
      } else {
        switch (key_code) {
          case MBTNL:
            Mouse.press(MOUSE_LEFT);
            return;
          case MBTNM:
            Mouse.press(MOUSE_MIDDLE);
            return;
          case MBTNR:
            Mouse.press(MOUSE_RIGHT);
            return;
          case MBTNB:
            Mouse.press(MOUSE_BACK);
            return;
          case MBTNF:
            Mouse.press(MOUSE_FORWARD);
            return;
          case RAISE:
            ++gCurrentLayer;
            return;
          case LEDMN:
            Leds::SetMinBrightness();
            return;
          case LEDMX:
            Leds::SetMaxBrightness();
            return;
          case LEDUP:
            Leds::IncreaseBrightness();
            return;
          case LEDDN:
            Leds::DecreaseBrightness();
            return;
          case XXXXX:
            return;
          default:
            ERROR("unexpected key code (%u) on press", key_code);
        }
      }
    }
  }

  void ProcessKeyReleaseEvent(uint8_t row, uint8_t col) {
    INFO("key release event: (%u, %u)", row, col);
    uint8_t layer = gKeyStateMatrix[row][col].ClearLastLayer();
    DETAIL("layer: %u", layer);
    uint16_t key_code = kLayout[layer][row][col];
    if (XXXXX == key_code) return;
    if (key_code >= FIRST_RESERVED_CODE) {
      Keyboard.release(key_code);
    } else {
      switch (key_code) {
        case MBTNL:
          Mouse.release(MOUSE_LEFT);
          return;
        case MBTNM:
          Mouse.release(MOUSE_MIDDLE);
          return;
        case MBTNR:
          Mouse.release(MOUSE_RIGHT);
          return;
        case MBTNB:
          Mouse.release(MOUSE_BACK);
          return;
        case MBTNF:
          Mouse.release(MOUSE_FORWARD);
          return;
        case RAISE:
          --gCurrentLayer;
          return;
        case LEDMN:
          return;
        case LEDMX:
          return;
        case LEDUP:
          return;
        case LEDDN:
          return;
        case XXXXX:
          return;
        default:
          ERROR("unexpected key code (%u) on release", key_code);
      }
    }
  }

  void Setup() {
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

  void Loop() {
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
