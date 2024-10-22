#ifndef HY_BASIC_ENDPOINT_H_
#define HY_BASIC_ENDPOINT_H_
#include <cstddef>
#include <cstdint>
#include "IPAddress.h"
#include "Net_type.hpp"
#include "detail/Endpoint.h"

namespace hy {
namespace net {

template <typename InternetProtocol>
class basic_endpoint {
 public:
  using protocol_type = InternetProtocol;
  using data_type = sockaddr_type;

  constexpr basic_endpoint() noexcept = default;
  constexpr basic_endpoint(const basic_endpoint&) noexcept = default;
  constexpr basic_endpoint(basic_endpoint&&) noexcept = default;
  constexpr basic_endpoint& operator=(const basic_endpoint&) = default;
  constexpr basic_endpoint& operator=(basic_endpoint&&) = default;
  constexpr ~basic_endpoint() = default;

  constexpr basic_endpoint(const InternetProtocol& internet_protocol,
                           sockaddr_port_type port) noexcept;

  constexpr basic_endpoint(const IPAddress& addr,
                           sockaddr_port_type port) noexcept;

  constexpr protocol_type get_protocol() const noexcept;

  constexpr data_type* data() noexcept;

  constexpr const data_type* data() const noexcept;

  constexpr sockaddr_port_type get_port() const noexcept;

  constexpr sockaddr_port_type set_port(sockaddr_port_type port) noexcept;

  constexpr IPAddress get_ipaddress() const noexcept;

  constexpr void set_ipaddress(const IPAddress& addr) noexcept;

  friend constexpr bool operator==(const basic_endpoint& lhs,
                                   const basic_endpoint& rhs) noexcept;

  friend constexpr bool operator<(const basic_endpoint& lhs,
                                  const basic_endpoint& rhs);

  friend constexpr std::ostream& operator<<(std::ostream& os,
                                            basic_endpoint& lhs);

 private:
  ::hy::net::detail::Endpoint impl_;
};
}  // namespace net
}  // namespace hy

namespace std {
template <typename InternetProtocol>
struct hash<hy::net::basic_endpoint<InternetProtocol>> {
  constexpr std::size_t operator()(
      const hy::net::basic_endpoint<InternetProtocol>& lhs) const noexcept {
    std::size_t hash1 = std::hash<asio::ip::address>()(lhs.get_ipaddress());
    std::size_t hash2 =
        std::hash<hy::net::sockaddr_port_type>()(lhs.get_port());
    return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
  }
}
}  // namespace std

#include "impl/Basic_endpoint_impl.hpp"

#endif  //HY_BASIC_ENDPOINT_H_