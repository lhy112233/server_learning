#ifndef IPADDRESSV4_H_
#define IPADDRESSV4_H_

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <tuple>
#include <string_view>
#include <utility>
#include "IPAddressException.h"
#include "Expected.h"

namespace hy {
namespace net {
class IPAddress;
class IPAddressV4;
class IPAddressV6;

/**
 * @brief "IPV4地址结构体"和"网络掩码"组成的一对结构
 */
using CIDRNetworkV4 = std::tuple<IPAddressV4, uint8_t>;

/**
 * @brief 存储ipv4地址的数组结构 
 */
using ByteArray4 = std::array<uint8_t, 4>;

class IPAddressV4 {
public:
  static constexpr std::size_t kMaxToFullyQualifiedSize =
      4 /*words*/ * 3 /*max chars per word*/ + 3 /*separators*/;

  /**
   * Returns true if the input string can be parsed as an IP address.
   */
  static bool validate(std::string_view ip) noexcept;

  /**
   * Create an IPAddressV4 instance from a uint32_t, using network byte
   * order(网络字节序)
   */
  static IPAddressV4 fromLong(uint32_t src);

    /**
   * Create an IPAddressV4 instance from a uint32_t, using host byte
   * order(本地字节序)
   */
  static IPAddressV4 fromLongHBO(uint32_t src);

  /**
   * Create a new IPAddressV4 from the provided string.
   *
   * Returns an IPAddressFormatError if the string is not a valid IP.
   */
  static Expected<IPAddressV4, IPAddressFormatError> 
  tryFromString(std::string_view str) noexcept;
}; // class IPAddressV4
} // namespace net
} // namespace hy



#endif  //IPADDRESSV4_H_