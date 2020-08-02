#include "Leds.h"

#include <cstdint>
#include <cmath>

#include <Arduino.h>

#include "LuminanceTable.h"

namespace Leds {
  const uint8_t kNumLedPins = 5;
  const uint8_t kLedPins[] = {3, 4, 6, 9, 10};

  uint8_t gBrightness = kMaxBrightness / 2;

  uint8_t GetBrightness() {
    return gBrightness;
  }

  static void UpdateBrightness() {
    for (int i = 0; i < kNumLedPins; ++i) {
      analogWrite(kLedPins[i], kLuminances[gBrightness]);
    }
  }

  void SetBrightness(uint8_t level) {
    gBrightness = min(level, kMaxBrightness);
    UpdateBrightness();
  }

  void IncreaseBrightness() {
    SetBrightness(gBrightness+1);
  }

  void DecreaseBrightness() {
    if (gBrightness > 0) {
      SetBrightness(gBrightness-1);
    }
  }

  void SetMinBrightness() {
    SetBrightness(0);
  }

  void SetMaxBrightness() {
    SetBrightness(kMaxBrightness);
  }

  void Setup() {
    for (int i = 0; i < kNumLedPins; ++i) {
      uint8_t pin = kLedPins[i];
      pinMode(pin, OUTPUT);
    }
    UpdateBrightness();
  }

  void Loop() {}
}  // namespace Leds
