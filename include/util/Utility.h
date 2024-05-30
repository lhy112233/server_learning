#ifndef UTILITY_H_
#define UTILITY_H_

#include <cstring>
#include <exception>
#include <memory>
#include <stdexcept>

namespace hy {
template <typename T, std::size_t Size = sizeof(T)>
void MemZero(T &obj, std::size_t size = Size) noexcept {
#ifdef __STDC_LIB_EXT1__
#include <string.h>
  memset_s(static_cast<void *>(std::addressof(obj)), 1, 0, size);
#else
  std::memset(static_cast<void *>(std::addressof(obj)), 0, size);
  if (*static_cast<const char *>(
          static_cast<const void *>(std::addressof(obj))) != 0) {
    std::terminate();
  } /// 明示后条件,确保memset不被编译器优化掉
#endif //__STDC_LIB_EXT1__
} // MemZero
} // namespace hy
#endif // UTILITY_H_