#ifndef HY_IPADDRESS_IMPL_HPP_
#define HY_IPADDRESS_IMPL_HPP_
#include "IPAddress.h"

namespace hy {
namespace net {

inline constexpr IPAddress::IPAddress(const IPAddressV4& addr) noexcept
    : ipv4{addr}, type_{IPAddressType::IPV4} {}

inline constexpr IPAddress::IPAddress(const IPAddressV6& addr) noexcept
    : ipv6{addr}, type_{IPAddressType::IPV6} {}

inline constexpr IPAddress& IPAddress::operator=(
    const IPAddressV4& addr) noexcept {
  ipv4 = addr;
  type_ = IPAddressType::IPV4;
  return *this;
}

inline constexpr IPAddress& IPAddress::operator=(
    const IPAddressV6& addr) noexcept {
  ipv6 = addr;
  type_ = IPAddressType::IPV6;
  return *this;
}

inline constexpr IPAddress::IPAddress(
    const hy::net::IPAddressV4::ip_type& addr) noexcept
    : ipv4{addr}, type_{IPAddressType::IPV4} {}

inline constexpr IPAddress::IPAddress(
    const hy::net::IPAddressV6::ip_type& addr,
    sockaddr_scope_id_type scope) noexcept
    : ipv6{addr}, type_{IPAddressType::IPV6} {}

inline constexpr IPAddress& IPAddress::operator=(
    const hy::net::IPAddressV4::ip_type& addr) noexcept {
  ipv4 = addr;
  type_ = IPAddressType::IPV4;
  return *this;
}

inline constexpr IPAddress& IPAddress::operator=(
    const hy::net::IPAddressV6::ip_type& addr) noexcept {
  ipv6 = addr;
  type_ = IPAddressType::IPV6;
  return *this;
}

inline constexpr std::string IPAddress::to_string() const {
  return is_v4() ? ipv4.to_string() : ipv6.to_string();
}

inline constexpr IPAddressV4 IPAddress::to_v4() const {
  return ipv4;
}

inline constexpr IPAddressV6 IPAddress::to_v6() const {
  return ipv6;
}

inline constexpr bool IPAddress::is_v4() const noexcept {
  return type_ == IPAddressType::IPV4;
}

inline constexpr bool IPAddress::is_v6() const noexcept {
  return type_ == IPAddressType::IPV6;
}

inline constexpr IPAddress::IPAddressType IPAddress::ip_type() const noexcept {
  return type_;
}

inline constexpr bool IPAddress::is_loopback() const noexcept {
  return is_v4() ? ipv4.is_loopback() : ipv6.is_loopback();
}

inline constexpr bool IPAddress::is_broadcast() const noexcept {
  return is_v4() ? ipv4.is_broadcast() : ipv6.is_broadcast();
}

inline constexpr bool IPAddress::is_unspecified() const noexcept {
  return is_v4() ? ipv4.is_unspecified() : ipv6.is_unspecified();
}

inline constexpr IPAddress IPAddress::from_string(std::string_view str,
                                                 std::uint16_t args) {
  auto addr = from_string_nothrow(str, args);
  if (addr.has_value()) {
    return addr.value();
  } else {
    throw std::system_error{addr.error()};
  }
}

inline constexpr std::expected<IPAddress, std::error_code>
IPAddress::from_string_nothrow(std::string_view str, std::size_t args) noexcept {
  std::expected<IPAddress, std::error_code> ret =
      IPAddressV4::from_string(str, std::nothrow);
  if (ret.has_value() ||
      ret.error() ==
          std::make_error_code(std::errc::address_family_not_supported)) {
    return ret;
  } else {
    ret = IPAddressV6::from_string(str, args, std::nothrow);
    return ret;
  }
};

inline constexpr bool operator==(const IPAddress& lhs,
                                 const IPAddress& rhs) noexcept {
  if (lhs.ip_type() != lhs.ip_type()) {
    return false;
  } else if (lhs.ip_type() == IPAddress::IPAddressType::IPV4) {
    return lhs.ipv4 == rhs.ipv4;
  } else {
    return lhs.ipv4 == rhs.ipv6;
  }
}

inline constexpr bool operator!=(const IPAddress& lhs,
                                 const IPAddress& rhs) noexcept {
  return !(lhs == rhs);
}

inline constexpr bool operator<(const IPAddress& lhs, const IPAddress& rhs) {
  if (lhs.ip_type() != rhs.ip_type()) {
    throw std::invalid_argument{"IPAddress's interargument is unsame type"};
  } else if (lhs.ip_type() == IPAddress::IPAddressType::IPV4) {
    return lhs.ipv4 < rhs.ipv4;
  } else {
    return lhs.ipv6 < rhs.ipv6;
  }
}

inline constexpr bool operator<=(const IPAddress& lhs, const IPAddress& rhs) {
  return lhs < rhs || lhs == rhs;
}

inline constexpr bool operator>(const IPAddress& lhs, const IPAddress& rhs) {
  return lhs <= rhs ? false : true;
}

inline constexpr bool operator>=(const IPAddress& lhs, const IPAddress& rhs) {
  return lhs < rhs ? false : true;
}

inline constexpr std::ostream& operator<<(std::ostream& os,
                                          const IPAddress& ip) {
  return os << ip.to_string();
}

/*广播*/

}  // namespace net
}  // namespace hy

#endif  // HY_IPADDRESS_IMPL_HPP_