#ifndef HY_IPADDRESSV4_H_
#define HY_IPADDRESSV4_H_

#include <array>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <span>
#include <stdexcept>
#include <string_view>
#include <system_error>
#include <type_traits>

#ifdef __linux
#include <netinet/in.h>
#endif

/*Forward declare*/
class IPAddress;
class IPAddressV4;
class IPAddressV6;
class MacAddress;

using CIDRNetworkV4 = std::pair<IPAddressV4, std::uint8_t>;

namespace hy {
namespace net {
  class IPAddressV4;
using ByteArray4 = std::array<std::uint8_t, 4>;

class IPAddressV4 final {
  friend class std::hash<hy::net::IPAddressV4>;

 public:
  using ip_type = typename ::in_addr;

  static constexpr std::size_t kMaxToFullyQualifiedSize =
      4 /*words*/ * 3 /*max chars per word*/ + 3 /*separators*/;

  constexpr IPAddressV4() noexcept;
  constexpr IPAddressV4(const IPAddressV4&) noexcept = default;
  constexpr IPAddressV4(IPAddressV4&&) noexcept = default;
  constexpr IPAddressV4& operator=(const IPAddressV4&) noexcept = default;
  constexpr IPAddressV4& operator=(IPAddressV4&&) noexcept = default;
  constexpr ~IPAddressV4() noexcept = default;

  constexpr IPAddressV4(std::span<const std::uint8_t, 4> byte) noexcept;
  explicit constexpr IPAddressV4(std::uint32_t ip) noexcept;
  explicit constexpr IPAddressV4(const ip_type& addr) noexcept;

  constexpr IPAddressV4& operator=(const ip_type&) noexcept;

  constexpr ByteArray4 toByte() const noexcept;
  constexpr std::string to_string() const;
  constexpr std::uint32_t toHBOLong() const noexcept;

  constexpr bool is_loopback() const noexcept;
  constexpr bool is_broadcast() const noexcept;
  constexpr bool is_unspecified() const noexcept;

  static constexpr IPAddressV4 from_string(std::string_view str);
  static constexpr std::expected<IPAddressV4, std::error_code> from_string(
      std::string_view str, std::nothrow_t) noexcept;

  friend constexpr bool operator<(const IPAddressV4& lhs,
                                  const IPAddressV4& rhs) noexcept;

  friend constexpr bool operator==(const IPAddressV4& lhs,
                                   const IPAddressV4& rhs) noexcept;

  friend constexpr bool operator!=(const IPAddressV4& lhs,
                                   const IPAddressV4& rhs) noexcept;

  friend constexpr bool operator<=(const IPAddressV4& lhs,
                                   const IPAddressV4& rhs) noexcept;

  friend constexpr bool operator>=(const IPAddressV4& lhs,
                                   const IPAddressV4& rhs) noexcept;

  friend constexpr bool operator>(const IPAddressV4& lhs,
                                  const IPAddressV4& rhs) noexcept;

 private:
  ip_type addr_;
};

template <typename... Args>
constexpr IPAddressV4 makeIPAddressV4(Args&&... args);
 

// template <typename... Args>
//     requires std::is_constructible_v<IPAddressV4, Args&&...>
// constexpr std::expected<IPAddressV4, std::error_code>
// makeIPAddressV4(Args&&...args, const std::nothrow_t&) noexcept;

constexpr std::ostream& operator<<(std::ostream& os, const IPAddressV4& ip);


}  // namespace net

}  // namespace hy

namespace std {
template <>
struct hash<hy::net::IPAddressV4> {
  std::size_t operator()(const hy::net::IPAddressV4& addr) const noexcept {
    return std::hash<std::uint32_t>()(addr.addr_.s_addr);
  }
};
}  // namespace std

#include "impl/IPAddressV4_impl.hpp"
#endif  // HY_IPADDRESSV4_H_