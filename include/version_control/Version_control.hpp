#ifndef HY_VERSION_CONTROL_HPP_
#define HY_VERSION_CONTROL_HPP_
#include "Preprocessor.h"

/*这expected已在多个地方使用，使用非大写expected怕会生出变故*/
/*Expected*/
// #if(__cplusplus < CPP23)
// #include "Expected_Tiny.hpp"
// #define expected ::hy::expected
// #define unexpected  ::hy::unexpected
// #define bad_expected_access ::hy::bad_expected_access
// #else
// #include <expected>
// #define expected ::std::expected
// #define unexpected  ::std::unexpected
// #define bad_expected_access ::std::bad_expected_access
// #endif  //CPP23

/*Format*/
#if(__cplusplus < CPP20)
#include "fmt/core.h"
#define FORMAT ::fmt::format
#else
#include <format>
#define FORMAT ::std::format
#endif  //CPP20



#endif  //HY_VERSION_CONTROL_HPP_