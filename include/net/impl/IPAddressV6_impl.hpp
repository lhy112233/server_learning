#ifndef HY_IPADDRESSV6_IMPL_HPP_
#define HY_IPADDRESSV6_IMPL_HPP_

#include <algorithm>

#include "IPAddressV6.h"

#ifdef __linux
#include <arpa/inet.h>
#endif

namespace hy {
namespace net {

inline constexpr IPAddressV6::IPAddressV6(
    std::span<const std::uint8_t, 16> byte, scope_type scope) noexcept
    : scope_{scope} {
  std::copy(byte.begin(), byte.end(),
            static_cast<std::uint8_t*>(addr_.s6_addr));
}

inline constexpr IPAddressV6::IPAddressV6() noexcept : scope_{0} {
  hy::MemZero(addr_);
}

inline constexpr IPAddressV6::IPAddressV6(const ip_type& addr,
                                          scope_type scope) noexcept
    : addr_{addr}, scope_(scope) {}

inline constexpr ByteArray16 IPAddressV6::toByte() const noexcept {
  ByteArray16 ret{0};
  std::memcpy(ret.data(), addr_.s6_addr, 16);
  return ret;
}

inline constexpr std::string IPAddressV6::to_string() const {
  constexpr std::size_t kMaxSize = 45;
  std::string ret(kMaxSize, 0);
  ::inet_ntop(AF_INET6, addr_.s6_addr, ret.data(), INET6_ADDRSTRLEN);
  return ret;
}

inline constexpr std::expected<IPAddressV6, std::error_code>
IPAddressV6::from_string(std::string_view str, scope_type scope,
                         const std::nothrow_t& tag) noexcept {
  ip_type addr;
  switch (::inet_pton(AF_INET6, str.data(), std::addressof(addr))) {
    case 1:
      return IPAddressV6{addr, scope};
      break;
    case 0:
      return std::unexpected<std::error_code>{
          std::make_error_code(std::errc::invalid_argument)};
    default:
      return std::unexpected<std::error_code>{
          std::make_error_code(std::errc::address_family_not_supported)};
      break;
  }
}

inline constexpr IPAddressV6 IPAddressV6::from_string(std::string_view str,
                                                      scope_type scope) {
  ip_type addr;
  switch (::inet_pton(AF_INET6, str.data(), std::addressof(addr))) {
    case 1:
      return IPAddressV6(addr, scope);
      break;
    case 0:
      throw std::system_error{
          std::make_error_code(std::errc::invalid_argument)};
    default:
      throw std::system_error{
          std::make_error_code(std::errc::address_family_not_supported)};
      break;
  }
}

/*::1*/
inline constexpr IPAddressV6 LoopbackV6 =
    IPAddressV6{ByteArray16{0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01}};
/*ff:ff:ff:ff:ff:ff*/
inline constexpr IPAddressV6 BroadcastV6 =
    IPAddressV6{ByteArray16{0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                            0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff}};

inline constexpr bool IPAddressV6::is_loopback() const noexcept {
  return LoopbackV6 == *this;
}

inline constexpr bool IPAddressV6::is_broadcast() const noexcept {
  return BroadcastV6 == *this;
}

inline constexpr bool IPAddressV6::is_unspecified() const noexcept {
  return IPAddressV6{} == *this;
}

// inline constexpr bool IPAddressV6::is_unspecified() const noexcept {
//   return false;
// }

// inline constexpr bool IPAddressV6::isLinkLocal() const noexcept {
//    auto ary = toByte();
//   return false;
// }

// inline constexpr bool IPAddressV6::isSiteLocal() const noexcept {
//   return false;
// }

inline constexpr bool operator==(const IPAddressV6& lhs,
                                 const IPAddressV6& rhs) noexcept {
  return lhs.toByte() == rhs.toByte();
}

inline constexpr bool operator!=(const IPAddressV6& lhs,
                                 const IPAddressV6& rhs) noexcept {
  return lhs == rhs ? false : true;
}

inline constexpr bool operator<(const IPAddressV6& lhs,
                                const IPAddressV6& rhs) noexcept {
  return lhs.toByte() < rhs.toByte();
}

inline constexpr bool operator>(const IPAddressV6& lhs,
                                const IPAddressV6& rhs) noexcept {
  return (lhs < rhs || lhs == rhs) ? false : true;
}

inline constexpr bool operator<=(const IPAddressV6& lhs,
                                 const IPAddressV6& rhs) noexcept {
  return lhs < rhs || lhs == rhs;
}

inline constexpr bool operator>=(const IPAddressV6& lhs,
                                 const IPAddressV6& rhs) noexcept {
  return lhs > rhs || lhs == rhs;
}

inline constexpr std::ostream& operator<<(std::ostream& os,
                                          const IPAddressV6& ip) {
  return os << ip.to_string();
}

}  // namespace net
}  // namespace hy

#endif  // HY_IPADDRESSV6_IMPL_HPP_