#include "Traits.hpp"
// #include "Utility.h"
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <utility>
#include <vector>
#include <iostream>

template<typename T>
struct A{
    // template<typename U = T ,typename = std::enable_if_t<std::is_same_v<U, int>>>
    // void fun(double){
    //     std::cerr << "fun_1\n";
    // }

    template<typename U = T, typename = std::enable_if_t<!std::is_same_v<int, U>>>
    void fun(int,...){
        std::cerr << "fun_2\n";
    }

};

    template<typename = void, typename...>
    void fun(){std::cerr << "Fun_1\n";}

    template<typename = void>
    void fun(){std::cerr << "Fun_2\n";}


int main() {
}