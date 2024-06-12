#ifndef HY_LOGLEVEL_H_
#define HY_LOGLEVEL_H_

#include <cstdint>
#include <ostream>
#include <string_view>
namespace hy {

/* 级别越高越重要*/
/* DEBUG 和 INFO 级别名称与级别是相反的*/
/* DEBUG0 < DEBUG1*/
enum class LogLevel : std::uint32_t {
  UNINITIALIZED = 0,
  NONE = 1,
  MIN_LEVEL = 1,

  /*debug日志级别*/
  DEBUG = 1000,
  DBG = 1000,
  /*debug日志细粒度级别*/
  DEBUG0 = 1999,
  DEBUG1 = 1998,
  DEBUG2 = 1997,
  DEBUG3 = 1996,
  DEBUG4 = 1995,
  DEBUG5 = 1994,
  DEBUG6 = 1993,
  DEBUG7 = 1992,
  DEBUG8 = 1991,
  DEBUG9 = 1990,

  /*info日志级别*/
  INFO = 2000,
  /*info日志细粒度级别*/
  INFO0 = 2999,
  INFO1 = 2998,
  INFO2 = 2997,
  INFO3 = 2996,
  INFO4 = 2995,
  INFO5 = 2994,
  INFO6 = 2993,
  INFO7 = 2992,
  INFO8 = 2991,
  INFO9 = 2990,

  /*warning日志级别*/
  WARN = 3000,
  WARNING = 3000,

  /*error日志级别*/
  ERROR = 4000,
  ERR = 4000,

  /*批判的日志级别*/
  CRITICAL = 5000,

  /*致命的日志级别*/
  FATAL = 0x7fffffff,

  MAX_LEVEL = 0x7fffffff,
}; // enum class LogLevel

inline constexpr LogLevel kDefaultLogLevel = LogLevel::INFO;
inline constexpr LogLevel kMinFatalLogLevel = LogLevel::FATAL;

inline constexpr LogLevel operator+(LogLevel level,
                                    std::uint32_t value) noexcept {
  return ((static_cast<std::uint32_t>(level) + value) >
          (static_cast<std::uint32_t>(LogLevel::MAX_LEVEL)))
             ? LogLevel::MAX_LEVEL
             : static_cast<LogLevel>(static_cast<std::uint32_t>(level) + value);
}

inline constexpr LogLevel &operator+=(LogLevel &level,
                                      std::uint32_t value) noexcept {
  level = level + value;
  return level;
}

inline constexpr LogLevel operator-(LogLevel level,
                                    std::uint32_t value) noexcept {
  return static_cast<LogLevel>(static_cast<std::uint32_t>(level) - value);
}

inline constexpr LogLevel &operator-=(LogLevel &level,
                                      std::uint32_t value) noexcept {
  level = level - value;
  return level;
}

LogLevel stringToLogLevel(std::string_view name);

std::string logLevelToString(LogLevel level);

std::ostream &operator<<(std::ostream &os, LogLevel level);

inline constexpr bool isLogLevelFatal(LogLevel level) noexcept {
  return static_cast<std::uint32_t>(level) >=
         static_cast<std::uint32_t>(LogLevel::FATAL);
}

} // namespace hy

#endif // HY_LOGLEVEL_H_