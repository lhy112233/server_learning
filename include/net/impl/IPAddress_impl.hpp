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
    const hy::net::IPAddressV4::address_type& addr) noexcept
    : ipv4{addr}, type_{IPAddressType::IPV4} {}

inline constexpr IPAddress::IPAddress(
    const hy::net::IPAddressV6::address_type& addr,
    std::uint16_t scope) noexcept
    : ipv6{addr}, type_{IPAddressType::IPV6} {}

inline constexpr IPAddress& IPAddress::operator=(
    const hy::net::IPAddressV4::address_type& addr) noexcept {
  ipv4 = addr;
  type_ = IPAddressType::IPV4;
  return *this;
}

inline constexpr IPAddress& IPAddress::operator=(
    const hy::net::IPAddressV6::address_type& addr) noexcept {
  ipv6 = addr;
  type_ = IPAddressType::IPV6;
  return *this;
}

inline constexpr std::string IPAddress::toString() const {
  return isV4() ? ipv4.toString() : ipv6.toString();
}

inline constexpr bool IPAddress::isV4() const noexcept {
  return type_ == IPAddressType::IPV4;
}

inline constexpr bool IPAddress::isV6() const noexcept {
  return type_ == IPAddressType::IPV6;
}

inline constexpr IPAddress::IPAddressType IPAddress::Type() const noexcept {
  return type_;
}

inline constexpr bool IPAddress::isLoopback() const noexcept {
  return isV4() ? ipv4.isLoopback() : ipv6.isLoopback();
}

inline constexpr bool IPAddress::isBroadcast() const noexcept {
  return isV4() ? ipv4.isBroadcast() : ipv6.isBroadcast();
}

inline constexpr bool IPAddress::isUnspecified() const noexcept {
  return isV4() ? ipv4.isUnspecified() : ipv6.isUnspecified();
}

inline constexpr IPAddress IPAddress::fromString(std::string_view str,
                                                 std::uint16_t args) {
  auto addr = fromStringNothrow(str, args);
  if (addr.has_value()) {
    return addr.value();
  } else {
    throw std::system_error{addr.error()};
  }
}

inline constexpr std::expected<IPAddress, std::error_code>
IPAddress::fromStringNothrow(std::string_view str, std::size_t args) noexcept {
  std::expected<IPAddress, std::error_code> ret =
      IPAddressV4::fromString(str, std::nothrow);
  if (ret.has_value() ||
      ret.error() ==
          std::make_error_code(std::errc::address_family_not_supported)) {
    return ret;
  } else {
    ret = IPAddressV6::fromString(str, args, std::nothrow);
    return ret;
  }
};

inline constexpr bool operator==(const IPAddress& lhs,
                                 const IPAddress& rhs) noexcept {
  if (lhs.Type() != lhs.Type()) {
    return false;
  } else if (lhs.Type() == IPAddress::IPAddressType::IPV4) {
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
  if (lhs.Type() != rhs.Type()) {
    throw std::invalid_argument{"IPAddress's interargument is unsame type"};
  } else if (lhs.Type() == IPAddress::IPAddressType::IPV4) {
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
  return os << ip.toString();
}

/*广播*/

}  // namespace net
}  // namespace hy

#endif  // HY_IPADDRESS_IMPL_HPP_