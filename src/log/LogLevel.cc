#include "LogLevel.h"
#include <array>
#include <cstdint>
#include <ostream>
#include <string_view>

namespace hy {

/*匿名命名空间,限制外部链接*/
namespace {
struct NumberedLevelInfo {
  LogLevel min;
  LogLevel max;
  const char *lower_prefix;
  const char *upper_prefix;
}; // struct NumberedLevelInfo

constexpr std::array<NumberedLevelInfo, 2> numbered_log_levels = {
    NumberedLevelInfo{LogLevel::DEBUG, LogLevel::DEBUG0, "dbg", "DBG"},
    NumberedLevelInfo{LogLevel::INFO, LogLevel::INFO0, "info", "INFO"}};
} // namespace

LogLevel stringToLogLevel(std::string_view name) {
  constexpr std::string_view lowercasePrefix{"loglevel::"};
  constexpr std::string_view wrapperPrefix{"loglevel("};


}

std::string logLevelToString(LogLevel level) {
  if (level == LogLevel::UNINITIALIZED) {
    return "UNINITIALIZED";
  } else if (level == LogLevel::NONE) {
    return "NONE";
  } else if (level == LogLevel::DBG) {
    return "DEBUG";
  } else if (level == LogLevel::INFO) {
    return "INFO";
  } else if (level == LogLevel::WARN) {
    return "WARN";
  } else if (level == LogLevel::ERR) {
    return "ERR";
  } else if (level == LogLevel::CRITICAL) {
    return "CRITICAL";
  } else if (level == LogLevel::FATAL) {
    return "FATAL";
  }

  for (const auto &info : numbered_log_levels) {
    if (static_cast<std::uint32_t>(level) <=
            static_cast<std::uint32_t>(info.max) &&
        static_cast<std::uint32_t>(level) >
            static_cast<std::uint32_t>(info.min)) {
      auto num = static_cast<std::uint32_t>(info.max) -
                 static_cast<std::uint32_t>(level);
      return hy::to<std::string>(info.upper_prefix, num);
    }
  }

  return folly::to<string>("LogLevel(", static_cast<std::uint32_t>(level), ")");
}

std::ostream &operator<<(std::ostream &os, LogLevel level) {
  os << logLevelToString(level);
  return os;
}

} // namespace hy