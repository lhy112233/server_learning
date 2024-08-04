#include "Traits.hpp"
// #include "Utility.h"
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>
#include <iostream>

    template<typename T, typename = std::enable_if_t<std::is_same_v<int, std::remove_cv_t<T>>>>
    void testFun(T i){
        std::cout << "Fun_1\n";
    }

    template<typename T, typename = std::enable_if_t<!std::is_same_v<int, std::remove_cv_t<T>>>, typename...>
    void testFun(T i){
        std::cout << "Fun_2\n";
    }

    void testFun(int i, ...){
        std::cout << "Fun_3\n";
    }

int main() {

testFun(1);
testFun("a");

}