#include "Thumbstick.h"

#include<math.h>
#include<stdint.h>

#include <ADC.h>

#include "Logging.h"

namespace Thumbstick {
  const uint8_t kPinX = A12;
  const uint8_t kPinY = A13;
  const uint8_t kSamplesPerReport = 100;
  const double kMinRadius = 20;

  ADC gAdc;
  bool gScrolling = false;

  void SetScrolling() {
    gScrolling = true;
  }

  void ClearScrolling() {
    gScrolling = false;
  }

  void Setup() {
    gAdc.startSynchronizedContinuous(kPinX, kPinY);
  }

  uint8_t gSampleCount = 0;
  double gReadingX = 0;
  double gReadingY = 0;
  void Loop() {
    ADC::Sync_result r = gAdc.readSynchronizedContinuous();
    gReadingX += r.result_adc0;
    gReadingY += r.result_adc1;
    if (++gSampleCount >= kSamplesPerReport) {
      double x = gReadingX / kSamplesPerReport - 500.0;
      double y = gReadingY / kSamplesPerReport - 500.0;
      double r = sqrt(x*x + y*y);
      // if (r > kMinRadius) {
      //   double scale = 0.01 * (1.0 - kMinRadius / r);
      //   x *= scale;
      //   y *= scale;
      //   if (gScrolling) {
      //     Mouse.move(0, 0, y, x);
      //   } else {
      //     Mouse.move(x, y, 0, 0);
      //   }
      // }
      Logging::Msg("Thumbstick reading: (%f, %f)\n", x, y);
      gReadingX = 0;
      gReadingY = 0;
      gSampleCount = 0;
    }
  }
}  // namespace Joystick
