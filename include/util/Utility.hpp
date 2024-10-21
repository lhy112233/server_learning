#ifndef UTILITY_H_
#define UTILITY_H_

#include <string.h>
#include <cstring>
#include <exception>
#include <functional>
#include <memory>
#include <stdexcept>
#include <utility>
#include <bit>
#include "Preprocessor.h"

namespace hy {
template <typename T, std::size_t Size = sizeof(T)>
void MemZero(T& obj, std::size_t size = Size) noexcept {
#ifdef __STDC_LIB_EXT1__
  memset_s(static_cast<void*>(std::addressof(obj)), 1, 0, size);
#else
  std::memset(static_cast<void*>(std::addressof(obj)), 0, size);
  if (*static_cast<const char*>(
          static_cast<const void*>(std::addressof(obj))) != 0) {
    std::terminate();
  }  /// 明示后条件,确保memset不被编译器优化掉
#endif  //__STDC_LIB_EXT1__
}  // MemZero

#if __cplusplus < CPP20
template <class T, class... Args>
constexpr T* construct_at(T* p, Args&&... args) {
  return ::new (static_cast<void*>(p)) T(std::forward<Args>(args)...);
}
#else
template<typename T, typename...Args>
constexpr T* construct_at(T* p, Args&&... args) {
  return ::std::construct_at(p, std::forward<Args>(args)...);
}

#endif  //CPP17

template <typename F, typename... Args>
void check(bool judge, F&& f, Args&&... args) noexcept(
    noexcept(std::invoke(std::forward<F>(f), std::forward<Args>(args)...))) {
  if (!judge) {
    std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
  }
}

template <typename F, typename... Args>
void check(int judge, F&& f, Args&&... args) noexcept(
    noexcept(std::invoke(std::forward<F>(f), std::forward<Args>(args)...))) {
  if (judge == -1) {
    std::invoke(std::forward<F>(f), std::forward<Args>(args)...);
  }
}

template<typename T>
inline constexpr auto network_to_host(const T& src) noexcept {
  if constexpr (std::endian::big == std::endian::native) {
    return src;
  } else  if constexpr(std::endian::little == std::endian::native) {
    return std::byteswap(src);
  } else {
    static_assert(false, "std::endian::native is no big or little.");
  }
}

template<typename T>
inline constexpr auto host_to_network(const T& src) noexcept {
  return network_to_host(src);
}



}  // namespace hy
#endif  // UTILITY_H_
