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
using namespace hy::net;


int main() {
    IPAddressV4 v4 = IPAddressV4::from_string("127.0.0.1");
    std::cout << v4.is_loopback();
}