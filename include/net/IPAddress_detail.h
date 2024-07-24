#ifndef HY_IPADDRESS_DETAIL_H_
#define HY_IPADDRESS_DETAIL_H_
#include <sys/socket.h>
#include <cstddef>
#include <string>
namespace hy {
namespace detail {
std::string familyNameStrDefault(sa_family_t family);

inline std::string familyNameStr(sa_family_t family) {
  switch (family) {
    case AF_INET:
      return "AF_INIT";
    case AF_INET6:
      return "AF_INIT6";
    case AF_UNSPEC:
      return "AF_UNSPEC";
    case AF_UNIX:
      return "AF_UNIX";
    default:
      return familyNameStrDefault(family);
      ;
  }
}

[[noreturn]] void getNthMSBitImplThrow(std::size_t bitCount,
                                       sa_family_t family);

template <typename IPAddrType>
inline bool getNthMSBitImpl(const IPAddrType& ip, std::size_t bitIndex,
                            sa_family_t family) {
  if (bitIndex >= ip.bitCount()) {
    getNthMSBitImplThrow(ip.bitCount, family);
  } else {
    return (ip.getNthMSByte(bitIndex / 8) & (0x80 >> (bitIndex % 8))) != 0;
  }
}
}  // namespace detail
}  // namespace hy

#endif  //HY_IPADDRESS_DETAIL_H_