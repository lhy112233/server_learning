#ifndef TRAITS_HPP_
#define TRAITS_HPP_
#include <cstring>
#include <type_traits>

namespace hy {

// 平替Folly的struct StrictConjunction,
//  但需要C++17,主要是因为折叠表达式的加入和模板模板形参的typename.
/*旧实现方式*/
// template <template <typename...> typename Strategy, typename... Ts>
// struct StrategyConjunction
//     : std::integral_constant<bool, (Strategy<Ts>::value && ...)> {};
/*新实现方式,具有短路判断的优点(由std::conjunction提供此特性)*/
template<template<typename...> typename Strategy,typename... Ts>
struct StrategyConjunction : std::conjunction<Strategy<Ts>...>{};

template <template <typename...> typename Strategy, typename... Ts>
inline constexpr bool StrategyConjunction_v =
    StrategyConjunction<Strategy, Ts...>::value;

/***************检测大小判断*************************/
// 判断两元素是否可以比较相等
template <typename T, typename U = T>
struct is_equality_comparable
    : std::is_convertible<decltype(std::declval<T>() == std::declval<U>()),
                          bool> {};

template <typename T>
inline constexpr bool is_equality_comparable_v =
    is_equality_comparable<T>::value;

// 判断两元素是否可以小于比较
template <typename T, typename U = T>
struct is_lesser_comparable
    : std::is_convertible<decltype(std::declval<T>() < std::declval<U>()),
                          bool> {};

template <typename T>
inline constexpr bool is_lesser_comparable_v = is_lesser_comparable<T>::value;

// 判断两元素是否能大于比较
template <typename T, typename U = T>
struct is_more_comparable
    : std::is_convertible<decltype(std::declval<T>() > std::declval<U>()),
                          bool> {};

template <typename T>
inline constexpr bool is_more_comparable_v = is_more_comparable<T>::value;

/*******************************************/

template <typename T,typename...Ts> struct MaxType {
  using type = T;
};

template <typename T1, typename T2, typename... Ts>
struct MaxType<T1, T2, Ts...> {
  using type = std::conditional_t<(sizeof(T1) >= sizeof(T2)),
                                  typename MaxType<T1, Ts...>::type,
                                  typename MaxType<T2, Ts...>::type>;
};

template <typename... Ts> using MaxType_t = typename MaxType<Ts...>::type;

} // namespace hy
#endif // TRAITS_HPP_