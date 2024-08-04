#include "IPAddressV6.h"
#include <fmt/core.h>
#include "IPAddressException.h"

namespace hy {
namespace net {
bool IPAddressV6::validate(std::string_view ip) noexcept{
    return tryFromString(ip).has_value();
}

IPAddressV6 IPAddressV6::fromBinary(const ByteArray16& bytes){
    auto maybeIP = tryFromBinary(bytes);
    if(!maybeIP.has_value()){
        throw IPAddressFormatException(fmt::format("Invalid IPv6 binary data: length must be 16 bytes, got {}", bytes.size()));
    }else {
    return maybeIP.value();
    }
}

expected<IPAddressV6, IPAddressFormatError> IPAddressV6::tryFromBinary(
      const ByteArray16& bytes) noexcept{
        IPAddressV6 addr;
        auto setResult = addr.trySetFromBinary(bytes);
        if(!setResult.has_value()){
            return unexpected(setResult.error());
        }
        return addr;
      }


}
}