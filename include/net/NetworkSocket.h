#ifndef HY_NETWORK_SOCKET_H_
#define HY_NETWORK_SOCKET_H_
#include <cstddef>
#include <functional>
#include <ostream>
#include "detail/SocketFileDescriptorMap.h"
namespace hy {
namespace net {

struct NetworkSocket {
#ifdef _WIN32

#else
  using native_handle_typpe = int;
  inline static constexpr native_handle_typpe invalid_handle_value = -1;
#endif  //_WIN32

  native_handle_typpe data;

  /*Constructors*/
  constexpr NetworkSocket() : data(invalid_handle_value) {}
  constexpr explicit NetworkSocket(native_handle_typpe d) : data{d} {}

  template <typename T>
  static NetworkSocket fromFd(T) = delete;

  static NetworkSocket fromFd(int fd) {
    return NetworkSocket{
        ::hy::net::detail::SocketFileDescriptorMap::fdToSocket(fd)};
  }

  int toFd() const {
    return ::hy::net::detail::SocketFileDescriptorMap::socketToFd(data);
  }

  /*Friends functions*/
  friend constexpr bool operator==(const NetworkSocket& a,
                                   const NetworkSocket& b) noexcept {
    return a.data == b.data;
  }

  friend constexpr bool operator!=(const NetworkSocket& a,
                                   const NetworkSocket& b) noexcept {
    return !(a == b);
  }
};

template <typename CharT, typename Traits>
inline std::basic_ostream<CharT, Traits>& operator<<(
    std::basic_ostream<CharT, Traits>& os, const NetworkSocket& addr) {
  os << "hy::net::NetworkSocket(" << addr.data << ")";
  return os;
}
}  //namespace net
}  //namespace hy

namespace std {
template <>
struct hash<hy::net::NetworkSocket> {
  std::size_t operator()(const hy::net::NetworkSocket& s) const noexcept {
    return std::hash<hy::net::NetworkSocket::native_handle_typpe>()(s.data);
  }
};  //struct hash<hy::net::NetworkSocket>
}  // namespace std

#endif  //HY_NETWORK_SOCKET_H_