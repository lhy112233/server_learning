#ifndef HY_IPADDRESSV4_H_
#define HY_IPADDRESSV4_H_

#include <netinet/in.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>
#include <ostream>
#include <string>
#include <string_view>
#include <tuple>
#include <utility>
#include "Expected_Tiny.hpp"
#include "IPAddressException.h"
#include "IPAddressV6.h"
#include "Unit.hpp"
#include "Utility.h"

namespace hy {
namespace net {
class IPAddress;
class IPAddressV4;
class IPAddressV6;

/**
 * @brief "IPV4地址结构体"和"网络掩码"组成的一对结构
 */
using CIDRNetworkV4 = std::pair<IPAddressV4, std::uint8_t>;

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

  static IPAddressV4 fromBinary(ByteArray4 bytes);

  /**
   * Create a new IPAddressV4 from the provided string.
   *
   * Returns an IPAddressFormatError if the string is not a valid IP.
   */
  static expected<IPAddressV4, IPAddressFormatError> tryFromString(
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
  static IPAddressV4 fromInverseArpaName(std::string_view arpaname);

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
  IPAddressV4(std::string_view addr);
  explicit IPAddressV4(const ByteArray4& addr) noexcept;
  explicit IPAddressV4(const in_addr addr) noexcept;

  /*Member functions*/
  /*Conversion*/
  IPAddressV6 createIPv6() const;

  IPAddressV6 getIPv6For6To4() const;

  uint32_t toLong() const { return toAddr().s_addr; }

  uint32_t toLongHBO() const { return ntohl(toLong()); }

  static constexpr std::size_t bitCount() noexcept { return 32; }

  std::string toJson() const;

  size_t hash() const;

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

  sockaddr_in toSockAddr() const;

  ByteArray4 toByteArray() const;

  std::string toFullQualified() const { return str(); }

  void toFullyQualifiedAppend(std::string& out) const;

  std::uint8_t version() const noexcept { return 4; }

  static ByteArray4 fetchMask(std::size_t numBits);

  static CIDRNetworkV4 longestCommonPrefix(const CIDRNetworkV4& one,
                                           const CIDRNetworkV4& two);

  static std::size_t byteCount() noexcept { return 4; }

  bool getNthMSBit(std::size_t bitIndex) const {
    return detail::getNthMSBitImpl(*this, bitIndex, AF_INET);
  }

  std::uint8_t getNthMSByte(std::size_t byteIndex) const;

  bool getNthLSBit(std::size_t bitIndex) const;

  std::uint8_t getNthLSByte(std::size_t byteIndex) const;

  const unsigned char* bytes() const { return addr_.bytes_.data(); }

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

  expected<Unit, IPAddressFormatError> trySetFromBinary(
      ByteArray4 bytes) noexcept;
};  // class IPAddressV4

std::ostream& operator<<(std::ostream& os, const IPAddressV4& addr);

void toAppend(const IPAddressV4& addr, std::string& result);

inline bool operator==(const IPAddressV4& addr_1, const IPAddressV4& addr_2) {
  return (addr_1.toLong() == addr_2.toLong());
}

inline bool operator!=(const IPAddressV4& addr_1, const IPAddressV4& addr_2) {
  return (addr_1.toLong() != addr_2.toLong());
}

inline bool operator<(const IPAddressV4& addr_1, const IPAddressV4& addr_2) {
  return (addr_1.toLong() < addr_2.toLong());
}

inline bool operator<=(const IPAddressV4& addr_1, const IPAddressV4& addr_2) {
  return ((addr_1 < addr_2) || (addr_1 == addr_2));
}

inline bool operator>(const IPAddressV4& addr_1, const IPAddressV4& addr_2) {
  return (addr_2 < addr_1);
}

inline bool operator>=(const IPAddressV4& addr_1, const IPAddressV4& addr_2) {
  return !(addr_1 < addr_2);
}
}  // namespace net
}  // namespace hy

namespace std {
template <>
struct hash<hy::net::IPAddressV4> {
  std::size_t operator()(const hy::net::IPAddressV4& addr) const {
    return addr.hash();
  }
}; //hash
}  // namespace std

#endif  //HY_IPADDRESSV4_H_