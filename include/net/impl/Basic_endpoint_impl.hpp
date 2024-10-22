#ifndef HY_BASIC_ENDPOINT_IMPL_HPP_
#define HY_BASIC_ENDPOINT_IMPL_HPP_
#include "Basic_endpoint.h"

template <typename InternetProtocol>
inline constexpr basic_endpoint<InternetProtocol>::basic_endpoint(
    const InternetProtocol& internet_protocol, sockaddr_port_type port) noexcept
    : impl_{internet_protocol.get_family(), port} {}

template <typename InternetProtocol>
inline constexpr hy::net::basic_endpoint<InternetProtocol>::basic_endpoint(
    const IPAddress& addr, sockaddr_port_type port) noexcept
    : impl_{addr, port} {}

template <typename InternetProtocol>
inline constexpr protocol_type
hy::net::basic_endpoint<InternetProtocol>::get_protocol() const noexcept {
  return impl_.is_v4() ? InternetProtocol::v4() : InternetProtocol::v6();
}

template <typename InternetProtocol>
inline constexpr data_type*
hy::net::basic_endpoint<InternetProtocol>::data() noexcept {
  return impl_.data();
}

template <typename InternetProtocol>
inline constexpr const data_type*
hy::net::basic_endpoint<InternetProtocol>::data() const noexcept {
  return impl_.data();
}

template <typename InternetProtocol>
inline constexpr sockaddr_port_type
hy::net::basic_endpoint<InternetProtocol>::get_port() const noexcept {
  return impl_.get_port();
}

template <typename InternetProtocol>
inline constexpr sockaddr_port_type
hy::net::basic_endpoint<InternetProtocol>::set_port(
    sockaddr_port_type port) noexcept {
  return impl_.set_port(port);
}

template <typename InternetProtocol>
inline constexpr IPAddress
hy::net::basic_endpoint<InternetProtocol>::get_ipaddress() const noexcept {
  return impl_.to_ipaddress();
}

template <typename InternetProtocol>
inline constexpr void hy::net::basic_endpoint<InternetProtocol>::set_ipaddress(
    const IPAddress& addr) noexcept {
  impl_ = IPAddress{addr, impl_.get_port()};
}

constexpr bool hy::net::operator==(const basic_endpoint& lhs,
                                   const basic_endpoint& rhs) noexcept {
  return lhs.impl_ == rhs.impl_;
}

constexpr bool hy::net::operator<(const basic_endpoint& lhs,
                                  const basic_endpoint& rhs) {
  return lhs.impl_ < rhs.impl_;
}

constexpr std::ostream& hy::net::operator<<(std::ostream& os,
                                            basic_endpoint& lhs) {
  return os << lhs.impl_.to_string();
}

#endif  //HY_BASIC_ENDPOINT_IMPL_HPP_