#ifndef HY_BASIC_ENDPOINT_H_
#define HY_BASIC_ENDPOINT_H_
#include <cstddef>
#include <cstdint>
#include "Net_type.hpp"

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
  ~constexpr basic_endpoint() = default;

  basic_endpoint(const InternetProtocol& internet_protocol,
                 port_type port) noexcept
      : impl_(internet_protocol.family(), port) {}

    

 private:
  ::hy::net::detail::Endpoint impl_;
};
}  // namespace net
}  // namespace hy

#endif  //HY_BASIC_ENDPOINT_H_