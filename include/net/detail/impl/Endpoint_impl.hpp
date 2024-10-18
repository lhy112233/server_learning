#ifndef ENDPOINT_IMPL_HPP_
#define ENDPOINT_IMPL_HPP_
#include "Endpoint.h"

namespace hy{
    namespace net{
        namespace detail{
            inline constexpr Endpoint::Endpoint(family_type family, port_type port) noexcept{

            }
inline constexpr Endpoint::Endpoint(const IPAddress & addr, port_type port) noexcept
{
}

inline constexpr sockaddr * Endpoint::data() noexcept
{
return nullptr;
}

inline constexpr const sockaddr * Endpoint::data() const noexcept
{
return nullptr;
}

inline constexpr std::size_t Endpoint::size() const noexcept
{
return std::size_t();
}

inline constexpr port_type Endpoint::get_port() const noexcept
{
return port_type();
}

inline constexpr void Endpoint::set_port(port_type port) noexcept
{
}

inline constexpr bool Endpoint::is_v4() const noexcept
{
return false;
}

inline constexpr bool Endpoint::is_v6() const noexcept
{
return false;
}

inline constexpr family_type Endpoint::get_family() const noexcept
{
return family_type();
}



        }
    }
}













#endif  //ENDPOINT_IMPL_HPP_