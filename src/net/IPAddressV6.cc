#include "IPAddressV6.h"
#include <fmt/core.h>
#include <cstddef>
#include <string_view>
#include "Version_control.hpp"
#include "IPAddressException.h"
#include "Unexpected.hpp"

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
  constexpr std::size_t kMaxSize = 45;

  if (str.size() < 2) {
    return unexpected<IPAddressFormatError>(
        IPAddressFormatError::INVALID_IP);
  }

  auto ip =
      str.front() == '[' && str.back() == ']' ? str.substr() : str.substr();
}


}  // namespace net
}  // namespace hy