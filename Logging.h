#ifndef __LOGGING_H_
#define __LOGGING_H_

#include <Arduino.h>

namespace Logging {
  void Init();
  void Scan();

  // not ISR-safe
  void Msg(const char* msg, ...);
  void Log(const char* level,
           const char* file,
           int line,
           const char* msg, ...);
}  // namespace Logging

// log levels
#define _LOGGING_SILENT 0
#define _LOGGING_CRITICAL 1
#define _LOGGING_ERROR 2
#define _LOGGING_WARN 3
#define _LOGGING_INFO 4
#define _LOGGING_DETAIL 5

// set this lower to compile away logging of higer levels
#define _LOGGING_VERBOSITY _LOGGING_DETAIL

#define _LOGGING_LOG(level, ...) Logging::Log(level, __FILE__, __LINE__, __VA_ARGS__)

#if _LOGGING_VERBOSITY >= _LOGGING_CRITICAL
#define CRITICAL(...) _LOGGING_LOG("CRITICAL", __VA_ARGS__);
#else
#define CRITICAL(...)
#endif

#if _LOGGING_VERBOSITY >= _LOGGING_ERROR
#define ERROR(...) _LOGGING_LOG("ERROR", __VA_ARGS__);
#else
#define ERROR(...)
#endif

#if _LOGGING_VERBOSITY >= _LOGGING_WARN
#define WARN(...) _LOGGING_LOG("WARN", __VA_ARGS__);
#else
#define WARN(...)
#endif

#if _LOGGING_VERBOSITY >= _LOGGING_INFO
#define INFO(...) _LOGGING_LOG("INFO", __VA_ARGS__);
#else
#define INFO(...)
#endif

#if _LOGGING_VERBOSITY >= _LOGGING_DETAIL
#define DETAIL(...) _LOGGING_LOG("DETAIL", __VA_ARGS__);
#else
#define DETAIL(...)
#endif

#endif // __LOGGING_H_
