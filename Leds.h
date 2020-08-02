#ifndef __LEDS_H_
#define __LEDS_H_

#include <cstdint>

namespace Leds {
  void Setup();
  void Loop();

  uint8_t GetBrightness();
  void SetBrightness(uint8_t level);
  void SetMinBrightness();
  void SetMaxBrightness();
  void IncreaseBrightness();
  void DecreaseBrightness();

  //void Animate(uint8_t id);
}  // namespace Leds
#endif // __LEDS_H_
