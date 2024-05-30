#ifndef PREPROCESSOR_H_
#define PREPROCESSOR_H_

#define CPP98 199711L
#define CPP11 201103L
#define CPP14 201402L
#define CPP17 201703L
#define CPP20 202002L
#define CPP23 202302L

#if(__cplusplus < CPP20)
#define LIKELY
#define UNLIKELY
#else
#define LIKELY [[likely]]
#define UNLIKELY [[unlikely]]
#endif  //(__cplusplus < CPP20)


/**
 * @brief 将s1和s2拼接在一起
 * @details https://zh.cppreference.com/w/cpp/preprocessor/replace
 */
#define HY_CONCATENATE_IMPL(s1, s2) s1##s2
#define HY_CONCATENATE(s1, s2) HY_CONCATENATE_IMPL(s1, s2)






#endif  //PREPROCESSOR_H_