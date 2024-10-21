#ifndef HY_NETTYPE_HPP_
#define HY_NETTYPE_HPP_

#ifdef __linux
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

namespace hy {
namespace net {
using sockaddr_type = ::sockaddr;
using sockaddr_v4_type = ::sockaddr_in;
using sockaddr_v6_type = ::sockaddr_in6;
using sockaddr_family_type = ::sa_family_t;
using in_addr_type = ::in_addr;
using in6_addr_type = ::in6_addr;
using addrinfo_type = ::addrinfo;
using sockaddr_port_type = ::in_port_t;
using sockaddr_scope_id_type = ::std::uint32_t;

inline constexpr sockaddr_family_type family_v4 = AF_INET;
inline constexpr sockaddr_family_type family_v6 = AF_INET6;
}  // namespace net
}  // namespace hy

#elif __win32
#include <windows.h>
#include <ws2def.h>

using addrinfo_type = ::addrinfo;

#else
/*待定*/
#endif

#endif  //HY_NETTYPE_HPP_