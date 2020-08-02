#include "Thumbstick.h"

#include<math.h>
#include<stdint.h>

#include <ADC.h>

#include "Logging.h"

namespace Thumbstick {
  const uint8_t kPinX = A13;
  const uint8_t kPinY = A12;

  bool gScrolling = false;

  void SetScrolling() {
    gScrolling = true;
  }

  void ClearScrolling() {
    gScrolling = false;
  }

  void Update(int32_t x, int32_t y) {
      DETAIL("Thumbstick reading: (%i, %i)", x, y);
  }

  ADC gAdc;

  void Setup() {
    gAdc.startSynchronizedContinuous(kPinY, kPinX);
  }

  void Loop() {
    ADC::Sync_result r = gAdc.readSynchronizedContinuous();
    Update(r.result_adc1, r.result_adc0);
  }
}  // namespace Joystick
