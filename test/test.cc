#include "Traits.hpp"
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
    expected<int,double> exp{1};
    std::cout << "测试" << exp.value();
    return exp;
}

int main() {
    fun();
}