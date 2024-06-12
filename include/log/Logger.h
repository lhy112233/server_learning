#ifndef HY_LOGGER_H_
#define HY_LOGGER_H_

#define HY_LOG(logger, level, ...)                                             \
  HY_LOG_IMPL(logger, ::hy::LogLevel::level, ::hy::LogStreamProcessor::APPEND, \
              ##_VA_ARGS__)

#endif // HY_LOGGER__H_