#ifndef HY_BASIC_ENDPOINT_H_
#define HY_BASIC_ENDPOINT_H_
#include <cstdint>
#include <cstddef>
#include "Net_type.hpp"


namespace hy{
    namespace net{
        
        template <typename InternetProtocol>
        class basic_endpoint {
            public:
            using protocol_type = InternetProtocol;
            using data_type = socket_addr_type;

            basic_endpoint() noexcept;




            private:
            ::hy::net::detail::Endpoint impl_;

        };
    }
}












#endif  //HY_BASIC_ENDPOINT_H_