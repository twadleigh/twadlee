#include "Thumbstick.h"

#include<math.h>
#include<stdint.h>

#include <Arduino.h>

#include <ADC.h>

#include "Logging.h"
#include "Matrix.h"

namespace Thumbstick {
  const uint64_t kMinReportIntervalMs = 1;  // max report rate for USB mice is 1 kHz.

  const double kMinReading = 0.0;
  const double kMaxReading = 1023.0;
  const double kReadingOrigin = 0.5 * (kMinReading + kMaxReading);
  const double kReadingRadius = 0.5 * (kMaxReading - kMinReading);
  const double kMinModThreshold = 0.15;
  const double kBlendingFactor = 0.00005;

  const double kMaxSpeed = 4000 * 1e-6;  // max speed (px/us)
  double gVx = 0.0;  // velocity estimate (px/us)
  double gVy = 0.0;
  double gDx = 0.0; // cummulative cursor position change
  double gDy = 0.0;
  double gLastVelocityUpdateTimeUs = 0;
  uint64_t gEarliestReportTimeMs = 0;  // earliest time to make a mouse report

  void NewReading(int32_t ix, int32_t iy) {
    DETAIL("Thumbstick reading: (%i, %i)", ix, iy);

    // normalize the reading to [-1, 1]
    double vx = (double(ix) - kReadingOrigin) / kReadingRadius;
    double vy = (double(iy) - kReadingOrigin) / kReadingRadius;

    // update the velocity estimate
    double speed = sqrt(vx * vx + vy * vy);
    if (speed < kMinModThreshold) {
      gVx = 0.0;
      gVy = 0.0;
    } else {
      double scale = -kMaxSpeed * kBlendingFactor * (1.0-kMinModThreshold/speed);
      gVx *= 1.0 - kBlendingFactor;
      gVy *= 1.0 - kBlendingFactor;
      gVx += scale * vx;
      gVy += scale * vy;
    }

    // accumulate position change
    uint64_t nowUs = micros();
    double dt = nowUs - gLastVelocityUpdateTimeUs;
    gLastVelocityUpdateTimeUs = nowUs;
    if (dt <= 0.0) return; // try again next time (probably hit rollover)
    gDx += dt * gVx;
    gDy += dt * gVy;

    // make a report, if needed
    uint64_t nowMs = millis();
    if (nowMs >= gEarliestReportTimeMs && (gDx * gDx > 0.25 || gDy * gDy >= 0.25)) {
      int8_t dx = round(min(max(-127.0, gDx), 127.0));
      int8_t dy = round(min(max(-127.0, gDy), 127.0));

      if (Matrix::GetCurrentLayer() == 0) {
        // moving
        Mouse.move(dx, dy);
      } else {
        // scrolling
        Mouse.move(0, 0, dy, dx);
      }

      // reset the cummmulative position change
      gDx = 0.0;
      gDy = 0.0;

      // set earliest time for next report
      gEarliestReportTimeMs = nowMs + kMinReportIntervalMs;
    }
  }

  ADC gAdc;
  const uint8_t kPinX = A13;
  const uint8_t kPinY = A12;
  void Setup() {
    gAdc.startSynchronizedContinuous(kPinY, kPinX);
    gLastVelocityUpdateTimeUs = micros();
  }

  void Loop() {
    ADC::Sync_result r = gAdc.readSynchronizedContinuous();
    NewReading(r.result_adc1, r.result_adc0);
  }
}  // namespace Joystick
