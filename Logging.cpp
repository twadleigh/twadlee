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

  uint8_t gVerbosity = _LOGGING_INFO;

  uint8_t GetVerbosity() {
    return gVerbosity;
  }

  void SetVerbosity(uint8_t verbosity) {
    verbosity = min(verbosity, _LOGGING_MAX_VERBOSITY);
    if (verbosity != gVerbosity) {
      INFO("Verbosity changing from %u to %u", gVerbosity, verbosity);
      gVerbosity = verbosity;
    }
  }

  void SetMinVerbosity() {
    SetVerbosity(_LOGGING_SILENT);
  }

  void SetMaxVerbosity() {
    SetVerbosity(_LOGGING_MAX_VERBOSITY);
  }

  void IncreaseVerbosity() {
    SetVerbosity(gVerbosity + 1);
  }

  void DecreaseVerbosity() {
    if (gVerbosity > _LOGGING_SILENT) SetVerbosity(gVerbosity-1);
  }

  static void vPrint(const char* format, va_list args) {
    char buf[2048];
    vsprintf(buf, format, args);
    Serial.print(buf);
  }

  void Print(const char* format, ...) {
    va_list args;
    va_start(args, format);
    vPrint(format, args);
    va_end(args);
  }

  void Log(uint8_t level,
           const char* label,
           const char* file,
           int line,
           const char* format, ...) {
    if (level <= gVerbosity) {
      double time = 0.001*millis();
      Print("[%10.3f s] %s: ", time, label);
      va_list args;
      va_start(args, format);
      vPrint(format, args);
      va_end(args);
      Print(" @ %s:%u\n", file, line);
    }
  }
}  // namespace Logging
