#include "IPAddressV4.h"
// #include "IPAddressV4.h"
// #include <arpa/inet.h>
// #include <fmt/core.h>
// #include <netinet/in.h>
// #include <sys/socket.h>
// #include <algorithm>
// #include <cstring>
// #include <memory>
// #include <string_view>
// #include "IPAddressException.h"

// namespace hy {
// namespace net {

// bool IPAddressV4::validate(std::string_view ip) noexcept {
//   return tryFromString(ip).has_value();
// }

// IPAddressV4 IPAddressV4::fromLong(uint32_t ip) {
//   in_addr addr;
//   addr.s_addr = ip;
//   return IPAddressV4(addr);
// }

// IPAddressV4 IPAddressV4::fromLongHBO(uint32_t ip) {
//   in_addr addr;
//   addr.s_addr = htonl(ip);
//   return IPAddressV4(addr);
// }

// std::expected<IPAddressV4, IPAddressFormatError> tryFromString(
//     std::string_view str) noexcept {
//   in_addr inAddr;
//   if (inet_pton(AF_INET, str.data(), std::addressof(inAddr)) != 1) {
//     // return makeUnexpected(IPAddressFormatError::INVALID_IP);
//   }
//   return IPAddressV4(inAddr);
// }

// ByteArray4 IPAddressV4::toBinary() const {
//   ByteArray4 ret;
//   std::memcpy(ret.data(), std::addressof(addr_), sizeof(addr_));
//   return ret;
// }

// IPAddressV4 IPAddressV4::fromInverseArpaName(std::string_view arpaname) {
//   if (arpaname.find(".in-addr.arpa")) {  //use ends_with function in C++20
//     arpaname.remove_suffix(sizeof(".in-addr.arpa"));
//   } else {
//     throw IPAddressFormatException(
//         fmt::format("input does not end with '.in-addr.arpa': '{}'", arpaname));
//   }
//   /*TODO*/
// }

// uint32_t IPAddressV4::toLong(std::string_view ip) {
//   auto str = ip.data();
//   in_addr addr;
//   if (inet_pton(AF_INET, str, std::addressof(addr))) {
//     throw IPAddressFormatException(
//         fmt::format("Can't convert invalid IP '{}' to long", ip));
//   }
//   return addr.s_addr;
// }

// uint32_t IPAddressV4::toLongHBO(std::string_view ip) {
//   return ntohl(IPAddressV4::toLong(ip));
// }

// IPAddressV4::IPAddressV4() = default;

// IPAddressV4::IPAddressV4(std::string_view addr) : addr_() {
//   if (inet_pton(AF_INET, addr.data(), std::addressof(addr_)) != 1) {
//     throw IPAddressFormatException(
//         fmt::format("Invalid IPv4 address '{}'", addr.data()));
//   }
// }

// IPAddressV4::IPAddressV4(const ByteArray4& addr) noexcept :addr_{addr} {}

// IPAddressV4::IPAddressV4(const in_addr src) noexcept : addr_(src) {}

// IPAddressV6 IPAddressV4::createIPv6() const{
//   /*TODO*/
// }

// }  //namespace net
// }  //namespace hy
