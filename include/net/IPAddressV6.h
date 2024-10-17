#ifndef HY_IPADDRESSV6_H_
#define HY_IPADDRESSV6_H_
/*依赖于C++23的std::expected,C++20的constexpr条件的进一步放松*/

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <expected>
#include <format>
#include <memory>
#include <new>
#include <ostream>
#include <span>
#include <string>
#include <string_view>
#include <system_error>
#include <utility>

#include "Unit.hpp"
#include "Utility.hpp"

#ifdef __linux
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#endif

namespace hy {
namespace net {

/*Forward declare*/
class IPAddress;
class IPAddressV4;
class IPAddressV6;
class MacAddress;

using CIDRNetworkV6 = std::pair<IPAddressV6, std::uint8_t>;

using ByteArray16 = std::array<std::uint8_t, 16>;

class IPAddressV6 final {
 public:
  using ip_type = ::in6_addr_type;
  using scope_type = std::uint16_t;

  constexpr IPAddressV6() noexcept;
  constexpr IPAddressV6(const IPAddressV6&) noexcept = default;
  constexpr IPAddressV6(IPAddressV6&&) noexcept = default;
  constexpr IPAddressV6& operator=(const IPAddressV6&) noexcept = default;
  constexpr IPAddressV6& operator=(IPAddressV6&&) noexcept = default;
  constexpr ~IPAddressV6() = default;

  constexpr IPAddressV6(std::span<const std::uint8_t, 16> byte,
                        scope_type scope = 0) noexcept;
  constexpr IPAddressV6(const ip_type& addr,
                        scope_type scope = 0) noexcept;

  constexpr scope_type getScope() const noexcept { return scope_; }
  constexpr void setScope(scope_type scope) noexcept { scope_ = scope; }

  constexpr ByteArray16 toByte() const noexcept;
  constexpr std::string to_string() const;

  constexpr bool is_loopback() const noexcept;
  constexpr bool is_broadcast() const noexcept;
  constexpr bool is_unspecified() const noexcept;

  constexpr static std::expected<IPAddressV6, std::error_code> from_string(
      std::string_view str, scope_type scope,
      const std::nothrow_t& tag) noexcept;

  constexpr static IPAddressV6 from_string(std::string_view str,
                                          scope_type scope = 0);

  friend constexpr bool operator<(const IPAddressV6& lhs,
                                  const IPAddressV6& rhs) noexcept;

  friend constexpr bool operator==(const IPAddressV6& lhs,
                                   const IPAddressV6& rhs) noexcept;

  friend constexpr bool operator>(const IPAddressV6& lhs,
                                  const IPAddressV6& rhs) noexcept;

  friend constexpr bool operator<=(const IPAddressV6& lhs,
                                   const IPAddressV6& rhs) noexcept;

  friend constexpr bool operator>=(const IPAddressV6& lhs,
                                   const IPAddressV6& rhs) noexcept;

  friend constexpr bool operator!=(const IPAddressV6& lhs,
                                   const IPAddressV6& rhs) noexcept;

 private:
  ip_type addr_{0};
  scope_type scope_{0};
};

// template<typename...Args>
// requires std::is_constructible_v<IPAddressV6, Args...>
// constexpr IPAddressV4 makeIPAddressV6(Args&&...args)
// noexcept(std::is_nothrow_constructible_v<IPAddressV6, Args...>);

// template<typename...Args>
// requires std::is_constructible_v<std::expected<IPAddressV6, std::error_code>,
// Args...> ||

constexpr std::ostream& operator<<(std::ostream& os, const IPAddressV6& ip);

}  // namespace net
}  // namespace hy

namespace std {
template <>
struct hash<hy::net::IPAddressV6> {
  constexpr std::size_t operator()(
      const hy::net::IPAddressV6& addr) const noexcept {
    auto bytes = addr.toByte();
    std::size_t result = static_cast<std::size_t>(addr.getScope());
    combine_4_bytes(result, &bytes[0]);
    combine_4_bytes(result, &bytes[4]);
    combine_4_bytes(result, &bytes[8]);
    combine_4_bytes(result, &bytes[12]);
    return result;
  }

 private:
  static constexpr void combine_4_bytes(std::size_t seed,
                                        const uint8_t* bytes) {
    const std::size_t bytes_hash = (static_cast<std::size_t>(bytes[0]) << 24) |
                                   (static_cast<std::size_t>(bytes[1]) << 16) |
                                   (static_cast<std::size_t>(bytes[2]) << 8) |
                                   (static_cast<std::size_t>(bytes[3]));
    seed ^= bytes_hash + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }
};
}  // namespace std

#include "impl/IPAddressV6_impl.hpp"

#endif  // HY_IPADDRESSV6_H_