#ifndef HY_TCP_H_
#define HY_TCP_H_
#include "Basic_endpoint.h"
#include "Net_type.hpp"

namespace hy {
namespace net {
class Tcp {
 public:
  using basic_endpoint<Tcp> endpoint;

  static constexpr Tcp v4() noexcept;

  static constexpr Tcp v6() noexcept;

  constexpr socket_protocol_type get_protocol() const noexcept;

  constexpr socket_type_type get_type() const noexcept;

  constexpr sockaddr_family_type get_family() const noexcept;

  friend constexpr bool operator==(const Tcp& lhs, const Tcp& rhs) noexcept;

  friend constexpr bool operator!=(const Tcp& lhs, const Tcp& rhs) noexcept;

 private:
  constexpr explicit Tcp(sockaddr_family_type family) noexcept;

  sockaddr_family_type family_;
};
}  // namespace net
}  // namespace hy

#endif  //HY_TCP_H_