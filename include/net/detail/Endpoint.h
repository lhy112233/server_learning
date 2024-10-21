#ifndef HY_ENDPOINT_H_
#define HY_ENDPOINT_H_

#include <cstddef>
#include <iostream>
#include "IPAddress.h"
#include "Net_type.hpp"

namespace hy {
namespace net {
namespace detail {
class Endpoint {
  static_assert(offsetof(sockaddr_type, sa_family) ==
                offsetof(sockaddr_v4_type, sin_family));
  static_assert(offsetof(sockaddr_type, sa_family) ==
                offsetof(sockaddr_v6_type, sin6_family));

 public:
  constexpr Endpoint(sockaddr_family_type family, sockaddr_port_type port);

  constexpr Endpoint(const IPAddress& addr, sockaddr_port_type port) noexcept;

  constexpr Endpoint(const Endpoint& rhs) noexcept = default;
  constexpr Endpoint(Endpoint&& rhs) noexcept = default;
  constexpr Endpoint& operator=(const Endpoint& rhs) noexcept = default;
  constexpr Endpoint& operator=(Endpoint&& rhs) noexcept = default;
  constexpr ~Endpoint() = default;

  constexpr sockaddr* data() noexcept;

  constexpr const sockaddr* data() const noexcept;

  constexpr std::size_t size() const noexcept;

  constexpr sockaddr_port_type get_port() const noexcept;

  constexpr void set_port(sockaddr_port_type port) noexcept;

  constexpr bool is_v4() const noexcept;

  constexpr bool is_v6() const noexcept;

  constexpr sockaddr_family_type get_family() const noexcept;

  friend constexpr bool operator==(const Endpoint& lhs,
                                   const Endpoint& rhs) noexcept;

  friend constexpr bool operator<(const Endpoint& lhs,
                                  const Endpoint& rhs) noexcept;

  friend constexpr std::ostream operator<<(std::ostream& os,
                                           const Endpoint& endpoint) noexcept;

 private:
  union {
    sockaddr_type base;
    sockaddr_v4_type ipv4;
    sockaddr_v6_type ipv6;
  } data_;
};

}  // namespace detail
}  // namespace net
}  // namespace hy

#include "impl/Endpoint_impl.hpp"

#endif  //HY_ENDPOINT_H_