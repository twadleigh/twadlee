#include "Leds.h"
#include "Logging.h"
#include "Matrix.h"
#include "Thumbstick.h"

void setup() {
  Logging::Setup();
  //Matrix::Setup();
  //Thumbstick::Setup();
  Leds::Setup();
}

void loop() {
  Logging::Loop();
  //Matrix::Loop();
  //Thumbstick::Loop();
  Leds::Loop();
}
