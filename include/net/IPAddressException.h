#ifndef IPADDRESSEXCEPTION_H_
#define IPADDRESSEXCEPTION_H_


#include <sys/socket.h>
#include <stdexcept>
#include "IPAddress_detail.h"
namespace hy{
    namespace net {
    
    enum class IPAddressFormatError{
        INVALID_IP,
        UNSUPPORTED_ADDR_FAMILY,
        NULL_SOCKADDR,
    };


    enum class CIDRNetworkError{
        INVALID_DEFAULT_CIDR,   
        INVALID_IP_SLASH_CIDR,  
        INVALID_IP,
        INVALID_CIDR,
        CIDR_MISMATCH,
    };

    class IPAddressFormatException : public std::runtime_error{
        public:
        using std::runtime_error::runtime_error;
    };

    class InvalidAddressFamilyException
    : public IPAddressFormatException{
        public:
        explicit InvalidAddressFamilyException(const char* msg)
        : IPAddressFormatException{msg} {}

        explicit InvalidAddressFamilyException(const std::string& msg) noexcept
        : IPAddressFormatException{msg} {}

        explicit InvalidAddressFamilyException(sa_family_t family) noexcept
        : InvalidAddressFamilyException{"Address family" + detail::familyNameStr(family) +
        " is not AF_INIT or AF_INT6"} {}
    };
    }   //namespace hy
}   //namespace hy



#endif  //IPADDRESSEXCEPTION_H_