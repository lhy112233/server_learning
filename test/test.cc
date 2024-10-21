#include <bit>
#include <cstddef>
#include <format>
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>
#include "Endpoint.h"
#include "IPAddress.h"
#include "IPAddressV4.h"
#include "IPAddressV6.h"
#include "Traits.hpp"
#include "Unexpected.hpp"
using namespace hy;
using namespace hy::net;

int main() {
  IPAddress addr_1 = IPAddressV6::from_string("1:12::0");
  hy::net::detail::Endpoint point_1{addr_1, 12};
    IPAddress addr_2 = IPAddressV6::from_string("1:12::0");
  hy::net::detail::Endpoint point_2{addr_2, 12};
  std::cout << (point_1 == point_2);
}