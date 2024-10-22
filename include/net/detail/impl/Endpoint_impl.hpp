#ifndef ENDPOINT_IMPL_HPP_
#define ENDPOINT_IMPL_HPP_
#include <cstring>
#include <format>
#include <stdexcept>
#include <system_error>
#include "Endpoint.h"
#include "Net_type.hpp"
#include "Utility.hpp"

namespace hy {
namespace net {
namespace detail {
inline constexpr Endpoint::Endpoint(sockaddr_family_type family,
                                    sockaddr_port_type port) {
  MemZero(data_);
  switch (family) {
    case family_v4:
      data_.ipv4.sin_family = family_v4;
      data_.ipv4.sin_port = host_to_network(port);
      break;
    case family_v6:
      data_.ipv6.sin6_family = family_v6;
      data_.ipv6.sin6_port = host_to_network(port);
      break;
    default:
      throw std::system_error{
          std::make_error_code(std::errc::address_family_not_supported)};
      break;
  }
}

inline constexpr Endpoint::Endpoint(const IPAddress& addr,
                                    sockaddr_port_type port) noexcept {
  if (addr.is_v4()) {
    data_.ipv4.sin_family = family_v4;
    data_.ipv4.sin_port = host_to_network(port);
    auto byte_array = addr.to_v4().toByte();
    std::memcpy(std::addressof(data_.ipv4.sin_addr), std::addressof(byte_array),
                sizeof(byte_array));
  } else {
    data_.ipv6.sin6_family = family_v6;
    data_.ipv6.sin6_port = host_to_network(port);
    auto byte_array = addr.to_v6().toByte();
    std::memcpy(std::addressof(data_.ipv6.sin6_addr),
                std::addressof(byte_array), sizeof(byte_array));
    data_.ipv6.sin6_scope_id = addr.to_v6().get_scope();
  }
}

inline constexpr sockaddr* Endpoint::data() noexcept {
  return std::addressof(data_.base);
}

inline constexpr const sockaddr* Endpoint::data() const noexcept {
  return std::addressof(data_.base);
}

inline constexpr std::size_t Endpoint::size() const noexcept {
  return is_v4() ? sizeof(sockaddr_v4_type) : sizeof(sockaddr_v6_type);
}

inline constexpr sockaddr_port_type Endpoint::get_port() const noexcept {
  return is_v4() ? network_to_host(data_.ipv4.sin_port)
                 : network_to_host(data_.ipv6.sin6_port);
}

inline constexpr void Endpoint::set_port(sockaddr_port_type port) noexcept {
  if (is_v4()) {
    data_.ipv4.sin_port = host_to_network(port);
  } else {
    data_.ipv6.sin6_port = host_to_network(port);
  }
}

inline constexpr bool Endpoint::is_v4() const noexcept {
  return data_.base.sa_family == family_v4;
}

inline constexpr bool Endpoint::is_v6() const noexcept {
  return data_.base.sa_family == family_v6;
}

inline constexpr sockaddr_family_type Endpoint::get_family() const noexcept {
  return data_.base.sa_family;
}

constexpr std::string Endpoint::to_string() const {
  if (is_v4()) {
    return std::format("{}:{}", IPAddressV4{data_.ipv4.sin_addr}.to_string(),
                       get_port());
  } else {
    return std::format("[{}]:{}", IPAddressV6{data_.ipv6.sin6_addr}.to_string(),
                       get_port());
  }
}

inline constexpr IPAddress Endpoint::to_ipaddress() const noexcept {
  if (is_v4()) {
    return IPAddress{data_.ipv4.sin_addr};
  } else {
    return IPAddress{data_.ipv6.sin6_addr, data_.ipv6.sin6_scope_id};
  }
}

constexpr std::ostream& operator<<(std::ostream& os,
                                   const Endpoint& endpoint) noexcept {
  return (os << endpoint.to_string());
}

constexpr bool operator==(const Endpoint& lhs, const Endpoint& rhs) noexcept {
  return lhs.get_port() == rhs.get_port() &&
         lhs.to_ipaddress() == rhs.to_ipaddress();
}

constexpr bool operator<(const Endpoint& lhs, const Endpoint& rhs) noexcept {
  return lhs.to_ipaddress() < rhs.to_ipaddress();
}

}  // namespace detail
}  // namespace net
}  // namespace hy

#endif  //ENDPOINT_IMPL_HPP_