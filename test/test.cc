#include "Traits.hpp"
#include "Unexpected.hpp"
// #include "Utility.h"
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>
#include <iostream>
#include <Expected_Tiny.hpp>
using namespace hy;


expected<int, double> fun(){
    expected<int,double> exp_1{1};
    auto exp_2 = expected<int, double>{hy::unexpected<double>{2.0}};
    swap(exp_1,exp_2);
    exp_1 = 11;
    std::cout << "测试" << exp_1.value();
    return exp_2;
}

int main() {
    fun();
}