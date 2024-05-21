#ifndef TRAITS_HPP_
#define TRAITS_HPP_
#include <type_traits>

//平替Folly的struct StrictConjunction,
// 但需要C++17,主要是因为折叠表达式的加入和模板模板形参的typename.
template <template <typename...> typename Strategy, typename... Ts>
struct StrategyConjunction
    : std::integral_constant<bool, (Strategy<Ts>::value && ...)> {};

template <template <typename...> typename Strategy, typename... Ts>
inline constexpr bool StrategyConjunction_v =
    StrategyConjunction<Strategy, Ts...>::value;

#endif // TRAITS_HPP_