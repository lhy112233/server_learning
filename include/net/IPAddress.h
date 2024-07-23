#ifndef IPADDRESS_H_
#define IPADDRESS_H_

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <utility>
#include <string>

#ifdef __linux
#include <sys/socket.h>

#endif //__linux

namespace hy {
namespace net {
    class IPAddress;
    using CIDRNetwork = std::pair<IPAddress,uint8_t>;

    class IPAddress{
        private:
        template<typename T>
        auto pick(T f) const{
            return isV4() ? f(asV4()) : isV6() ? f(asV6()) : f(asNode());
        }   //auto pick(T f)

        class IPAddressNode{
            public:
            bool empty() const{ return true;}
            std::size_t bitCount() const {return 0;}
            std::string toJson() const {return "{family:'AF_UNSPEC', addr:'', hash:0}";}
            std::size_t hash() const {return std::hash<uint64_t>{}(0);}
            bool isLoopback() const {
                throw InvalidAddressFamilyException{""};
            }







            std::string str() const noexcept(noexcept(std::string{""})) {return "";}
            std::string toFullyQualified() const noexcept(noexcept(std::string{""})) {return"";}
            std::uint8_t version() const noexcept {return 0;}
            const unsigned char* bytes() const noexcept{return nullptr;}
        };   //class IPAddressNode

        public:
        static bool validate(std::string_view ip) noexcept;

        static IPAddressV4(const IPAddress& addr);

        static IPAddressV6 createIPv6(const IPAddress* addr);

        static Expected<CIDRNetwork, >

    };  //class IPAddress

}    //namespace net
} // namespace hy

#endif  //IPADDRESS_H_