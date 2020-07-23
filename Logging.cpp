#include "Logging.h"

#include <cstdio>
#include <cstdarg>

namespace Logging {
  void Setup() {
    Serial.begin(9600);
    delay(1000);
    INFO("Logging initialized");
  }

  void Loop() {}

  static void vMsg(const char* format, va_list args) {
    char buf[2048];
    vsprintf(buf, format, args);
    Serial.println(buf);
  }

  void Msg(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vMsg(format, args);
    va_end(args);
  }

  void Log(const char* level,
           const char* file,
           int line,
           const char* format, ...) {
    double time = 0.001*millis();
    Msg("+vvv %s vvv %s:%i", level, file, line);
    Serial.print("> ");
    va_list args;
    va_start(args, format);
    vMsg(format, args);
    va_end(args);
    Msg("+^^^ %s ^^^ %f", level, time);
  }
}  // namespace Logging
