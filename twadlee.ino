#include "Leds.h"
#include "Logging.h"
#include "Matrix.h"

void setup() {
  Logging::Init();
  Matrix::Init();
  Leds::Init();
}

void loop() {
  Logging::Scan();
  Matrix::Scan();
  Leds::Scan();
}
