#include "IPAddressV6.h"
#include <fmt/core.h>
#include <sys/socket.h>
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstring>
#include <memory>
#include <string_view>
#include "IPAddressException.h"
#include "Unexpected.hpp"
#include "Version_control.hpp"

#ifdef __linux
  #include <netdb.h>
#endif  //__linux

namespace hy {
namespace net {
std::ostream& operator<<(std::ostream& os, const IPAddressV6& addr) {
  os << addr.hash();
  return os;
}

void toAppend(const IPAddressV6& addr, std::string& result) {
  result.append(addr.str());
}

bool IPAddressV6::validate(std::string_view ip) noexcept {
  return tryFromString(ip).has_value();
}

IPAddressV6::IPAddressV6() = default;

IPAddressV6::IPAddressV6(std::string_view addr) {
  auto maybeIP = tryFromString(addr);
  if (!maybeIP.has_value()) {
    throw IPAddressFormatException(FORMAT("Invalid IPv6 address '{}'", addr));
  }
  *this = maybeIP.value();
}

expected<IPAddressV6, IPAddressFormatError> IPAddressV6::tryFromString(
    std::string_view str) noexcept {
  /*strlen(ffff:ffff:ffff:ffff:ffff:ffff:255.255.255.255) == 45*/
  constexpr std::size_t kMaxSize = 45;

  /* strlen(::) == 2 */
  if (str.size() < 2) {
    return unexpected<IPAddressFormatError>(IPAddressFormatError::INVALID_IP);
  }

  auto ip = ((str.front() == '[') && (str.back() == ']'))
                ? str.substr(1, std::min(str.size() - 2, kMaxSize))
                : str.substr(0, std::min(str.size(), kMaxSize));
  
  /*sizeof('/0') == 1*/
  std::array<char, kMaxSize + 1> ipBuffer;
  std::copy(ip.cbegin(), ip.cend(), ipBuffer.begin());
  ipBuffer[ip.size()] = '\0';

  struct addrinfo* result;
  struct addrinfo hints;
  std::memset(std::addressof(hints), 0, sizeof(hints));
  hints.ai_family = AF_INET6;
  hints.ai_socktype = SOCK_STREAM;
  hints.ai_flags = AI_NUMERICHOST;
  /*::getaddrinfo成功时返回 0 */
  if (::getaddrinfo(ipBuffer.data(), nullptr, &hints, &result) == 0) {
    SCOPE_EXIT {
      ::freeaddrinfo(result);
    };
    const struct sockaddr_in6* sa =
        reinterpret_cast<struct sockaddr_in6*>(result->ai_addr);
    return IPAddressV6(*sa);
  }
  return unexpected<IPAddressFormatError>{IPAddressFormatError::INVALID_IP};
}

}  // namespace net
}  // namespace hy