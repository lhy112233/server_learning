#include <netinet/in.h>
#include "IPAddressV4.h"
#include <arpa/inet.h>
#include <memory>

namespace hy {
namespace net {

bool IPAddressV4::validate(std::string_view ip) noexcept {
  return tryFromString(ip).hasValue();
}

IPAddressV4 IPAddressV4::fromLong(uint32_t ip) {
  in_addr addr;
  addr.s_addr = ip;
  return IPAddressV4(addr);
}

IPAddressV4 IPAddressV4::fromLongHBO(uint32_t ip) {
  in_addr addr;
  addr.s_addr = htonl(ip);
  return IPAddressV4(addr);
}

Expected<IPAddressV4, IPAddressFormatError> tryFromString(
    std::string_view str) noexcept {
  in_addr inAddr;
  if (inet_pton(AF_INET, str.data(), std::addressof(inAddr)) != 1) {
    return makeUnexpected(IPAddressFormatError::INVALID_IP);
  }
  return IPAddressV4(inAddr);
}

IPAddressV4::IPAddressV4(const in_addr src) noexcept : addr_(src) {}




}  //namespace net
}  //namespace hy