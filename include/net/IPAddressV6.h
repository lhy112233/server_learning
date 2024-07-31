#ifndef HY_IPADDRESSV6_H_
#define HY_IPADDRESSV6_H_
#include <array>
#include <cstdint>
#include <utility>
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

 private:
};  //class IPAddressV6

}  //namespace net
}  //namespace hy

#endif  //HY_IPADDRESSV6_H_