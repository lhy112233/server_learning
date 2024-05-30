#ifndef IPADDRESSEXCEPTION_H_
#define IPADDRESSEXCEPTION_H_


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
    }   //namespace hy
}   //namespace hy



#endif  //IPADDRESSEXCEPTION_H_