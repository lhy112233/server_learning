#ifndef HY_IPADDRESSV4_H_
#define HY_IPADDRESSV4_H_

#include <netinet/in.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>
#include <string_view>
#include <tuple>
#include <utility>
#include "IPAddressException.h"
#include "Unit.hpp"
#include "Utility.h"
#include "Expected_Tiny.hpp"


namespace hy {
namespace net {
class IPAddress;
class IPAddressV4;
class IPAddressV6;

/**
 * @brief "IPV4地址结构体"和"网络掩码"组成的一对结构
 */
using CIDRNetworkV4 = std::tuple<IPAddressV4, std::uint8_t>;

/**
 * @brief 存储ipv4地址的数组结构 
 */
using ByteArray4 = std::array<uint8_t, 4>;

class IPAddressV4 {
 public:
  /*Static member functions*/
  /**
   * Max size of std::string returned by toFullyQualified()
   */
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
  static IPAddressV4 fromLong(uint32_t ip);

  /**
   * Create an IPAddressV4 instance from a uint32_t, using host byte
   * order(本地字节序)
   */
  static IPAddressV4 fromLongHBO(uint32_t ip);


  static IPAddressV4 fromBinary(ByteArray4, bytes);

  /**
   * Create a new IPAddressV4 from the provided string.
   *
   * Returns an IPAddressFormatError if the string is not a valid IP.
   */
  template <typename, typename>
  class Expected;
  static Expected<IPAddressV4, IPAddressFormatError> tryFromString(
      std::string_view str) noexcept;

  /**
   * Returns the address as a ByteRange.
   */
  ByteArray4 toBinary() const;

  /**
   * Create a new IPAddressV4 from a `in-addr.arpa` representation of an IP
   * address.
   *
   * @throws IPAddressFormatException if the input is not a valid in-addr.arpa
   * representation
   */
  static IPAddressV4 fromInverseArpaName(const std::string& arpaname);

  /**
   * Convert a IPv4 address string to a long, in network byte order.
   */
  static uint32_t toLong(std::string_view ip);

  /**
   * Convert a IPv4 address string to a long, in host byte order.
   *
   * This is slightly slower than toLong()
   */
  static uint32_t toLongHBO(std::string_view ip);

  /*Constructors*/
  IPAddressV4();
  IPAddressV4(std::string_view addr) noexcept;
  explicit IPAddressV4(const ByteArray4& addr) noexcept;
  explicit IPAddressV4(const in_addr addr) noexcept;

  /*Member functions*/
  /*Conversion*/
  IPAddressV6 createIPv6() const;

  IPAddressV6 getIPv6For6To4() const;

  uint32_t toLong() const { return toAddr().s_addr; }

  uint32_t toLongHBO() const { return ntohl(toLong()); }

  std::string toJson() const;

  /*Getters*/
  static constexpr size_t bitCount() { return 32; }

  size_t hash() const ;

  bool inSubnet(std::string_view cidrNetwork) const;


  bool inSubnet(const IPAddressV4& subnet, uint8_t cidr) const {
    return inSubnetWithMask(subnet, fetchMask(cidr));
  }

  bool inSubnetWithMask(const IPAddressV4& subnet, const ByteArray4 mask) const;

  bool isLoopback() const;

  bool isLinkLocal() const;

  bool isNonroutable() const;

  bool isPrivate() const;

  bool isMulticast() const;

  bool empty() const {
    constexpr auto zero = ByteArray4{{}};
    return 0 == std::memcmp(bytes(), zero.data(), zero.size());
  }

  bool isLinkLocalBroadcast() const {
    return (INADDR_BROADCAST == toLongHBO());
  }

    IPAddressV4 mask(size_t numBits) const;    

    std::string str() const;    

    std::string toInverseArpaName() const;    

    in_addr toAddr() const { return addr_.inAddr_; }

 private:
  union AddressStorage {
    static_assert(sizeof(in_addr) == sizeof(ByteArray4),
                  "size of in_addr and ByteArray4 are different");
    /*Constructor*/
    AddressStorage() { MemZero(*this); }
    explicit AddressStorage(const ByteArray4 bytes) : bytes_(bytes) {}
    explicit AddressStorage(const in_addr addr) : inAddr_(addr) {}
    /*Data*/
    in_addr inAddr_;
    ByteArray4 bytes_;
  };

  AddressStorage addr_;

  Expected<Unit, IPAddressFormatError> trySetFromBinary(
      ByteArray4 bytes) noexcept;
};  // class IPAddressV4
}  // namespace net
}  // namespace hy

#endif  //HY_IPADDRESSV4_H_