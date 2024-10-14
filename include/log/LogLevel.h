#ifndef HY_LOGLEVEL_H_
#define HY_LOGLEVEL_H_

#include <stdexcept>
#include <string>
#include <string_view>
namespace hy {

enum class LogLevel {
  DEBUG,
  INFO,
  WARRNING,
  ERROR,
  FATAL,
};

inline constexpr LogLevel DEBUG = LogLevel::DEBUG;
inline constexpr LogLevel INFO = LogLevel::INFO;
inline constexpr LogLevel WARRNING = LogLevel::WARRNING;
inline constexpr LogLevel ERROR = LogLevel::ERROR;
inline constexpr LogLevel FATAL = LogLevel::FATAL;

inline constexpr std::string LogLevelToString(LogLevel log_level) {
  using namespace std::literals;
  switch (log_level) {
  case LogLevel::DEBUG:
    return "DEBUG"s;
  case LogLevel::INFO:
    return "INFO"s;
  case LogLevel::WARRNING:
    return "WARRNING"s;
  case LogLevel::ERROR:
    return "ERROR"s;
  case LogLevel::FATAL:
    return "FATAL"s;
    break;
  }
  throw std::runtime_error("enum class LogLevel convert to string have error");
}

inline constexpr LogLevel stringToLogLevel(std::string_view str) {
  using namespace std::literals;
  if (str == "DEBUG"sv) {
    return DEBUG;
  } else if (str == "INFO"sv) {
    return INFO;
  } else if (str == "WARRNING"sv) {
    return WARRNING;
  } else if (str == "ERROR"sv) {
    return ERROR;
  } else if (str == "FATAL"sv) {
    return FATAL;
  }
  throw std::runtime_error("string convert to enum class LogLevel have error");
}

} // namespace hy

#endif // HY_LOGLEVEL_H_