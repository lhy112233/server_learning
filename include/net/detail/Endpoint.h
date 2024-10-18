#ifndef HY_ENDPOINT_H_
#define HY_ENDPOINT_H_

#include <iostream>
#include "IPAddress.h"
#include "Net_type.hpp"

namespace hy {
namespace net {
namespace detail {
class Endpoint {
 public:
  using family_type = ::sockaddr_family_type;
  using port_type = ::sockaddr_port_type;

  constexpr Endpoint(family_type family, port_type port) noexcept;

  constexpr Endpoint(const IPAddress& addr, port_type port) noexcept;

  constexpr Endpoint(const Endpoint& rhs) noexcept = default;
  constexpr Endpoint(Endpoint&& rhs) noexcept = default;
  constexpr Endpoint& operator=(const Endpoint& rhs) noexcept = default;
  constexpr Endpoint& operator=(Endpoint&& rhs) noexcept = default;
  constexpr ~Endpoint() = default;

  constexpr sockaddr* data() noexcept;

  constexpr const sockaddr* data() const noexcept;

  constexpr std::size_t size() const noexcept;

  constexpr port_type get_port() const noexcept;

  constexpr void set_port(port_type port) noexcept;

  constexpr bool is_v4() const noexcept;

  constexpr bool is_v6() const noexcept;

  constexpr family_type get_family() const noexcept;

  friend constexpr bool operator==(const Endpoint& lhs,
                                   const Endpoint& rhs) noexcept;

  friend constexpr bool operator<(const Endpoint& lhs,
                                  const Endpoint& rhs) noexcept;

  friend constexpr std::ostream operator<<(std::ostream& os,
                                           const Endpoint& endpoint) noexcept;

 private:
  union {
    ::sockaddr_type base;
    ::sockaddr_v6_type v6_;
    ::sockaddr_v4_type v4_;
  };
}

}  // namespace detail
}  // namespace net
}  // namespace hy

#endif  //HY_ENDPOINT_H_