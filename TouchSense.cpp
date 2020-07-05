#include "TouchSense.h"

#include <cstdint>

static const uint8_t kNumTouchSensePins = 8;
static const uint8_t kTouchSensePins[] = {15, 16, 17, 18, 19, 25, 32, 33};

namespace TouchSense {
  void Init() {}
  void Scan() {}
}  // namespace TouchSense
