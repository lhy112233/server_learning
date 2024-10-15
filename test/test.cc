#include "Traits.hpp"
#include "Unexpected.hpp"
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>
#include <iostream>
#include <format>
#include <bit>
#include "IPAddressV6.h"
#include "IPAddressV4.h"
#include "IPAddress.h"
using namespace hy;



int main() {
    // std::cout << std::format("test{}/n",__cplusplus);
    // std::cout << std::format("__cpp_concepts: {}",__cpp_concepts);
    // std::system_error err{std::make_error_code(std::errc::no_message)};
    // std::cout << err.what();
    hy::net::IPAddressV6 addr = hy::net::IPAddressV6::from_string("2001:0b28:f23f:f005:0000:0000:0000:000a");
    // hy::net::IPAddressV6 addr_2 = hy::net::IPAddressV6::from_string("100::100");
    std::cout << addr.to_string();
}