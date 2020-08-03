#ifndef __LOGGING_H_
#define __LOGGING_H_

#include <Arduino.h>

// log levels
#define _LOGGING_SILENT 0
#define _LOGGING_CRITICAL 1
#define _LOGGING_ERROR 2
#define _LOGGING_WARNING 3
#define _LOGGING_INFO 4
#define _LOGGING_DETAIL 5

// set this lower to compile away logging of higer levels
#define _LOGGING_MAX_VERBOSITY _LOGGING_DETAIL

namespace Logging {
  void Setup();
  void Loop();

  void SetVerbosity(uint8_t);
  uint8_t GetVerbosity();
  void SetMinVerbosity();
  void SetMaxVerbosity();
  void IncreaseVerbosity();
  void DecreaseVerbosity();

  // not ISR-safe
  void Print(const char* msg, ...);
  void Log(uint8_t level,
           const char* label,
           const char* file,
           int line,
           const char* msg, ...);
}  // namespace Logging


#define _LOGGING_LOG(level, lbl, ...) Logging::Log(level, lbl, __FILE__, __LINE__, __VA_ARGS__)

#if _LOGGING_MAX_VERBOSITY >= _LOGGING_CRITICAL
#define CRITICAL(...) _LOGGING_LOG(_LOGGING_CRITICAL, "CRITICAL", __VA_ARGS__);
#else
#define CRITICAL(...)
#endif

#if _LOGGING_MAX_VERBOSITY >= _LOGGING_ERROR
#define ERROR(...) _LOGGING_LOG(_LOGGING_ERROR, "ERROR", __VA_ARGS__);
#else
#define ERROR(...)
#endif

#if _LOGGING_MAX_VERBOSITY >= _LOGGING_WARN
#define WARNING(...) _LOGGING_LOG(_LOGGING_WARNING, "WARNING", __VA_ARGS__);
#else
#define WARNING(...)
#endif

#if _LOGGING_MAX_VERBOSITY >= _LOGGING_INFO
#define INFO(...) _LOGGING_LOG(_LOGGING_INFO, "INFO", __VA_ARGS__);
#else
#define INFO(...)
#endif

#if _LOGGING_MAX_VERBOSITY >= _LOGGING_DETAIL
#define DETAIL(...) _LOGGING_LOG(_LOGGING_DETAIL, "DETAIL", __VA_ARGS__);
#else
#define DETAIL(...)
#endif

#endif // __LOGGING_H_
