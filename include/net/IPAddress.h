#ifndef IPADDRESS_H_
#define IPADDRESS_H_

#include <compare>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <utility>

#include "IPAddressV4.h"
#include "IPAddressV6.h"

#include "Net_type.hpp"

namespace hy {
namespace net {
/*Forward declare*/
class IPAddress;
class IPAddressV4;
class IPAddressV6;
class MacAddress;

using CIDRNetworkV6 = std::pair<IPAddressV6, std::uint8_t>;

class IPAddress {
  static_assert(std::is_trivially_destructible_v<IPAddressV4> &&
                    std::is_trivially_destructible_v<IPAddressV6>,
                "destructible must be a trivially function.");
  friend struct std::hash<hy::net::IPAddress>;

 public:
  enum class IPAddressType {
    IPV4,
    IPV6,
  };

  IPAddress() = delete;
  constexpr IPAddress(const IPAddress&) noexcept = default;
  constexpr IPAddress(IPAddress&&) noexcept = default;
  constexpr IPAddress& operator=(const IPAddress&) noexcept = default;
  constexpr IPAddress& operator=(IPAddress&&) noexcept = default;
  constexpr ~IPAddress() noexcept = default;

  constexpr IPAddress(const IPAddressV4& addr) noexcept;
  constexpr IPAddress(const IPAddressV6& addr) noexcept;
  constexpr IPAddress& operator=(const IPAddressV4& addr) noexcept;
  constexpr IPAddress& operator=(const IPAddressV6& addr) noexcept;

  constexpr IPAddress(const hy::net::IPAddressV4::ip_type& addr) noexcept;
  constexpr IPAddress(const hy::net::IPAddressV6::ip_type& addr,
                      sockaddr_scope_id_type scope) noexcept;
  constexpr IPAddress& operator=(
      const hy::net::IPAddressV4::ip_type& addr) noexcept;
  constexpr IPAddress& operator=(
      const hy::net::IPAddressV6::ip_type& addr) noexcept;

  constexpr std::string to_string() const;

  constexpr IPAddressV4 to_v4() const;
  constexpr IPAddressV6 to_v6() const;

  constexpr bool is_v4() const noexcept;
  constexpr bool is_v6() const noexcept;
  constexpr IPAddressType ip_type() const noexcept;

  constexpr bool is_loopback() const noexcept;
  constexpr bool is_broadcast() const noexcept;
  constexpr bool is_unspecified() const noexcept;

  static constexpr IPAddress from_string(std::string_view str,
                                        std::uint16_t args = 0);
  static constexpr std::expected<IPAddress, std::error_code> from_string_nothrow(
      std::string_view str, std::size_t args) noexcept;

  friend constexpr bool operator==(const IPAddress& lhs,
                                   const IPAddress& rhs) noexcept;
  friend constexpr bool operator!=(const IPAddress& lhs,
                                   const IPAddress& rhs) noexcept;
  friend constexpr bool operator<(const IPAddress& lhs, const IPAddress& rhs);
  friend constexpr bool operator<=(const IPAddress& lhs, const IPAddress& rhs);
  friend constexpr bool operator>(const IPAddress& lhs, const IPAddress& rhs);
  friend constexpr bool operator>=(const IPAddress& lhs, const IPAddress& rhs);

 private:
  IPAddressType type_;

  union {
    IPAddressV4 ipv4;
    IPAddressV6 ipv6;
  };
};  // class IPAddress

constexpr std::ostream& operator<<(std::ostream& os, const IPAddress& ip);

// template<typename...Args>
// constexpr IPAddress makeIPAddress(Args&&...args) noexcept(noexcept())
// requires requires{

// };

}  // namespace net
}  // namespace hy

namespace std {
template <>
struct hash<hy::net::IPAddress> {
 public:
  constexpr std::size_t operator()(hy::net::IPAddress addr) const
      noexcept(noexcept(std::hash<hy::net::IPAddressV4>{}(addr.ipv4)) &&
               noexcept(std::hash<hy::net::IPAddressV6>{}(addr.ipv6))) {
    switch (addr.type_) {
      case hy::net::IPAddress::IPAddressType::IPV4:
        std::hash<hy::net::IPAddressV4>{}(addr.ipv4);
        break;
      case hy::net::IPAddress::IPAddressType::IPV6:
        return std::hash<hy::net::IPAddressV6>{}(addr.ipv6);
        break;
    }
  }
};
}  // namespace std

#include "impl/IPAddress_impl.hpp"
#endif  // IPADDRESS_H_