#ifndef __LEDS_H_
#define __LEDS_H_

#include <cstdint>

namespace Leds {
  void Setup();
  void Loop();

  extern const uint8_t kMaxBrightnessLevel;
  uint8_t GetBrightnessLevel();
  void SetBrightnessLevel(uint8_t level);

  void Animate(uint8_t id);
}  // namespace Leds
#endif // __LEDS_H_
