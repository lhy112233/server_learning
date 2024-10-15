#ifndef HY_BASIC_ENDPOINT_H_
#define HY_BASIC_ENDPOINT_H_
#include <cstdint>
#include <cstddef>


namespace hy{
    namespace net{
        using port_type = std::uint16_t;
        
        template <typename InternetProtocol>
        class basic_endpoint {
            public:


            basic_endpoint() noexcept;


            private:
            hy::net::port_type port_;

        };
    }
}












#endif  //HY_BASIC_ENDPOINT_H_