#ifndef HY_FILE_UTIL_DETAIL_H_
#define HY_FILE_UTIL_DETAIL_H_
#include <sys/types.h>
#include <cerrno>
#include <string>
#include <string_view>
namespace hy {
namespace detail {
#ifdef __linux
/*多次调用防止因为由于系统中断引起的失败*/
template <typename F, typename... Args>
inline ::ssize_t wrapNoEintr(F f, Args... args) {
  ::ssize_t r;
  do {
    r = f(args...);
  } while (r == -1 && errno == EINTR);
  return r;
}

inline void incr(::ssize_t) {}

template <typename Offset>
inline void incr(::ssize_t n, Offset& offset) {
  offset += static_cast<Offset>(n);
}

/*执行f直至完全执行完*/
template <typename F, typename... Offset>
ssize_t wrapFull(F f, int fd, void* buf, size_t count, Offset... offset) {
  char* b = static_cast<char*>(buf);
  ssize_t totalBytes = 0;
  ssize_t r;
  do {
    r = f(fd, b, count, offset...);
    if (r == -1) {
      if (errno == EINTR) {
        continue;
      }
      return r;
    }

    totalBytes += r;
    b += r;
    count -= r;
    incr(r, offset...);
  } while (r != 0 && count);  // 0 means EOF

  return totalBytes;
}

std::string getTemporaryFilePathString(std::string_view file_path,
                                       std::string_view temporary_directory);

#endif  //__linux
}  // namespace detail
}  // namespace hy

#endif  //HY_FILE_UTIL_DETAIL_H_