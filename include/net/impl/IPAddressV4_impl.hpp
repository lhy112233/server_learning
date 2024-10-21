#ifndef HY_IPADDRESSV4_IMPL_HPP_
#define HY_IPADDRESSV4_IMPL_HPP_

#include <algorithm>

#include "IPAddressV4.h"
#include "Utility.hpp"

#ifdef __linux
#include <arpa/inet.h>
#endif

namespace hy {
namespace net {
inline constexpr IPAddressV4::IPAddressV4() noexcept { addr_.s_addr = 0; }

inline constexpr IPAddressV4::IPAddressV4(
    std::span<const std::uint8_t, 4> byte) noexcept {
  std::memcpy(std::addressof(addr_.s_addr), byte.data(), sizeof(addr_.s_addr));
}

inline constexpr IPAddressV4::IPAddressV4(std::uint32_t ip) noexcept {
  // in_addr_t  == std::uint32_t
  addr_.s_addr = hy::host_to_network(ip);
}

inline constexpr IPAddressV4::IPAddressV4(const ip_type& addr) noexcept
    : addr_{addr} {}

inline constexpr IPAddressV4& IPAddressV4::operator=(
    const ip_type& addr) noexcept {
  addr_ = addr;
  return *this;
}

inline constexpr ByteArray4 IPAddressV4::toByte() const noexcept {
  static_assert(sizeof(ByteArray4) == sizeof(decltype(addr_.s_addr)),
                "内存布局不一致");
  ByteArray4 ret{0};
  std::memcpy(ret.data(), std::addressof(addr_.s_addr), sizeof(addr_.s_addr));
  return ret;
}

inline constexpr std::string IPAddressV4::to_string() const {
  std::string ret(kMaxToFullyQualifiedSize, 0);
  ::inet_ntop(AF_INET, std::addressof(addr_), ret.data(), INET_ADDRSTRLEN);
  return ret;
}

inline constexpr std::uint32_t IPAddressV4::toHBOLong() const noexcept {
  return hy::network_to_host(addr_.s_addr);
}

inline constexpr bool IPAddressV4::is_loopback() const noexcept {
  return (toHBOLong() & 0xFF000000) == 0x7F000000;
}

inline constexpr bool IPAddressV4::is_broadcast() const noexcept {
  return (toHBOLong() & 0xFFFFFFFF) == 0xFFFFFFFF;
}

inline constexpr bool IPAddressV4::is_unspecified() const noexcept {
  return toHBOLong() == 0x0;
}

inline constexpr IPAddressV4 IPAddressV4::from_string(std::string_view str) {
  ip_type addr;
  switch (::inet_pton(AF_INET, str.data(), std::addressof(addr))) {
    case 1:
      return IPAddressV4(addr);
      break;
    case 0:
      throw std::system_error{
          std::make_error_code(std::errc::invalid_argument)};
      break;
    default:
      throw std::system_error{
          std::make_error_code(std::errc::address_family_not_supported)};
      break;
  }
}

inline constexpr std::expected<IPAddressV4, std::error_code>
IPAddressV4::from_string(std::string_view str, std::nothrow_t) noexcept {
  ip_type addr;
  switch (::inet_pton(AF_INET, str.data(), std::addressof(addr))) {
    case 1:
      return std::expected<IPAddressV4, std::error_code>{addr};
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

inline constexpr bool operator<(const IPAddressV4& lhs,
                                const IPAddressV4& rhs) noexcept {
  return lhs.toByte() < rhs.toByte();
}

inline constexpr bool operator==(const IPAddressV4& lhs,
                                 const IPAddressV4& rhs) noexcept {
  return lhs.toByte() == rhs.toByte();
}

inline constexpr bool operator!=(const IPAddressV4& lhs,
                                 const IPAddressV4& rhs) noexcept {
  return lhs == rhs ? false : true;
}

inline constexpr bool operator>=(const IPAddressV4& lhs,
                                 const IPAddressV4& rhs) noexcept {
  return (lhs < rhs) ? false : true;
}

inline constexpr bool operator<=(const IPAddressV4& lhs,
                                 const IPAddressV4& rhs) noexcept {
  return ((lhs < rhs) || (lhs == rhs)) ? true : false;
}

inline constexpr bool operator>(const IPAddressV4& lhs,
                                const IPAddressV4& rhs) noexcept {
  return (lhs <= rhs) ? false : true;
}

inline constexpr std::ostream& operator<<(std::ostream& os,
                                          const IPAddressV4& ip) {
  return os << ip.to_string();
}

/*127.0.0.1*/
inline constexpr IPAddressV4 LoopbackV4 = IPAddressV4{0x7F000001};
/*256.256.256.256*/
inline constexpr IPAddressV4 BroadcastV4 = IPAddressV4{0xFFFFFFFF};

// template <typename... Args>
// inline constexpr IPAddressV4 makeIPAddressV4(Args&&... args) noexcept(
//     std::is_nothrow_constructible_v<IPAddressV4, Args&&...>) {
//   return IPAddressV4(std::forward<Args>...args);
// }

// template <typename... Args>
// constexpr std::expected<IPAddressV4, std::error_code> makeIPAddressV4(
//     Args&&... args, std::nothrow_t) noexcept {
//   return std::expected<IPAddressV4, std::error_code>();
// }

// template <typename... Args>
//     requires std::is_constructible_v<IPAddressV4, Args&&...>
// constexpr std::expected<IPAddressV4, std::error_code>
// makeIPAddressV4(Args&&...args, const std::nothrow_t&) noexcept{
//   return IPAddressV4{std::forward<Args>...args, std::nothrow};
// }

}  // namespace net
}  // namespace hy

#endif  // HY_IPADDRESSV4_IMPL_HPP_
