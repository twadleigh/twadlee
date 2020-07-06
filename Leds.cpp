#include "Leds.h"

#include <cstdint>
#include <cmath>

#include <Arduino.h>

namespace Leds {
  const uint8_t kNumLedPins = 5;
  const uint8_t kLedPins[] = {5, 20, 21, 22, 23};

  uint8_t gBrightnessLevel = 32;

  uint8_t GetBrightnessLevel() {
    return gBrightnessLevel;
  }

  static void UpdateBrightnessLevel() {
    for (int i = 0; i < kNumLedPins; ++i) {
      analogWrite(kLedPins[i], gBrightnessLevel);
    }
  }

  void SetBrightnessLevel(uint8_t level) {
    gBrightnessLevel = level;
    UpdateBrightnessLevel();
  }

  void Init() {
    for (int i = 0; i < kNumLedPins; ++i) {
      uint8_t pin = kLedPins[i];
      pinMode(pin, OUTPUT);
    }
    UpdateBrightnessLevel();
  }

  void Scan() {}
}  // namespace Leds
