#include "Traits.hpp"
// #include "Utility.h"
#include <cstddef>
#include <iostream>
#include <type_traits>
#include <typeinfo>
#include <vector>

int main() {
  std::cerr<< hy::StrategyConjunction_v<std::is_copy_assignable,int,int,double>;

}