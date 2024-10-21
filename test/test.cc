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
#include "Endpoint.h"
using namespace hy;
using namespace hy::net;


int main() {
    IPAddress addr = IPAddressV4::from_string("1.1.1.1");
    hy::net::detail::Endpoint point{addr,12};
}