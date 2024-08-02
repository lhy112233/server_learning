#ifndef HY_IPADDRESSV6_H_
#define HY_IPADDRESSV6_H_
#include <netinet/in.h>
#include <sys/types.h>
#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <utility>
#include "Expected_Tiny.hpp"
#include "IPAddressException.h"
#include "Unit.hpp"
#include "Utility.h"
namespace hy {
namespace net {
/*Forward declare*/
class IPAddress;
class IPAddressV4;
class IPAddressV6;
class MacAddress;

using CIDRNetworkV6 = std::pair<IPAddressV6, std::uint8_t>;

using ByteArray16 = std::array<std::uint8_t, 16>;

class IPAddressV6 final {
 public:
  enum class Type {
    TEREDO,
    T6TO4,
    NORMAL,
  };

  enum class LinkLocalTag {
    LINK_LOCAL,
  };

  inline static constexpr std::uint32_t PREFIX_TEREDO = 0x20010000;

  inline static constexpr uint32_t PREFIX_6TO4 = 0x2002;

  inline static constexpr size_t kToFullyQualifiedSize =
      8 /*words*/ * 4 /*hex chars per word*/ + 7 /*separators*/;

  static bool validate(std::string_view ip) noexcept;

  static IPAddressV6 fromBinary(const ByteArray16& bytes);

  static expected<IPAddressV6, IPAddressFormatError> tryFromBinary(
      const ByteArray16& bytes) noexcept;

  static expected<IPAddressV6, IPAddressFormatError> tryFromString(
      std::string_view str) noexcept;

  static IPAddressV6 fromInverseArpaName(std::string_view arpaname);

  ByteArray16 toBinary() const { return addr_.bytes_; }

  /*Constructors*/
  IPAddressV6();

  explicit IPAddressV6(std::string_view addr);

  explicit IPAddressV6(const ByteArray16& src) noexcept : addr_{src} {}

  explicit IPAddressV6(const in6_addr& src) noexcept : addr_{src} {}

  explicit IPAddressV6(const sockaddr_in6& src) noexcept;

  IPAddressV6(LinkLocalTag tag, MacAddress mac);

  IPAddressV4 createIPv4() const;

  IPAddressV4 getIPv4For6To4() const;

  bool is6To4() const {return type() == IPAddressV6::Type::T6TO4;}

  bool isTeredo() const { return type() == IPAddressV6::Type::TEREDO; }

  bool isIPv4Mapped() const;

  Type type() const;

  static constexpr size_t bitCount() { return 128; }

  std::string toJson() const;

  size_t hash() const;

  bool inSubnet(std::string_view cidrNetwork) const;

  bool inSubnet(const IPAddressV6& subnet, uint8_t cidr) const {
    return inSubnetWithMask(subnet, fetchMask(cidr));
  }

  bool inSubnetWithMask(
      const IPAddressV6& subnet, const ByteArray16& mask) const;

  bool isLoopback() const;

  bool isNonroutable() const { return !isRoutable(); }

  bool isRoutable() const;

  bool isPrivate() const;

  bool isLinkLocal() const;










  std::uint8_t getNthMSByte(size_t byteIndex) const;

  bool getNthLSBit(std::size_t bitInndex) const {
    return getNthMSBit(bitCount() - bitInndex - 1);
  }

  std::uint8_t getNthLSByte(std::size_t byteIndex) const {
    return getNthMSByte(byteCount() - byteIndex - 1);
  }

  const auto bytes() const noexcept { return addr_.in6Addr_.s6_addr; }

 protected:
  bool inBinarySubnet(const std::array<std::uint8_t, 2> addr,
                      std::size_t numBits) const;

 private:
  auto tie() const { return std::tie(addr_.bytes_, scope_); }

 public:
  /*Friends*/
  friend inline bool operator==(const IPAddressV6& lhs,
                                const IPAddressV6& rhs) {
    return lhs.tie() == rhs.tie();
  }

  friend inline bool operator!=(const IPAddressV6& lhs,
                                const IPAddressV6& rhs) {
    return !(lhs == rhs);
  }

  friend inline bool operator<(const IPAddressV6& lhs, const IPAddressV6& rhs) {
    return lhs.tie() < rhs.tie();
  }

  friend inline bool operator<=(const IPAddressV6& lhs,
                                const IPAddressV6& rhs) {
    return (lhs < rhs) || (lhs == rhs);
  }

  friend inline bool operator>(const IPAddressV6& lhs, const IPAddressV6& rhs) {
    return !(lhs <= rhs);
  }

  friend inline bool operator>=(const IPAddressV6& lhs,
                                const IPAddressV6& rhs) {
    return !(lhs < rhs);
  }

 private:
  union AddressStorage {
    in6_addr in6Addr_;
    ByteArray16 bytes_;
    AddressStorage() { MemZero(*this); }
    explicit AddressStorage(const in6_addr& addr) : in6Addr_(addr) {}
    explicit AddressStorage(const ByteArray16& bytes) : bytes_(bytes) {}
    explicit AddressStorage(const MacAddress& mac);
  } addr_;

  std::uint16_t scope_{0};

  expected<Unit, IPAddressFormatException> trySetFromBinary(
      const ByteArray16& bytes) noexcept;
};  //class IPAddressV6

}  //namespace net
}  //namespace hy

#endif  //HY_IPADDRESSV6_H_