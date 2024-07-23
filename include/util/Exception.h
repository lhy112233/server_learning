#ifndef HY_EXCEPTION_H_
#define HY_EXCEPTION_H_
#include <cstddef>
#include <string>
#include <type_traits>
#include <utility>
namespace hy {
template <typename E>
[[noreturn]] void throw_exception(E&& e) {
  throw std::forward<E>(e);
}

template <typename E>
[[noreturn]] void terminate_with(E&& e) {
  throw_exception(std::forward<E>(e));
}

namespace detail {
struct throw_exception_arg_array_ {
  template <typename R>
  using v = std::remove_extent_t<std::remove_reference_t<R>>;
  template <typename R>
  using apply = std::enable_if_t<std::is_same_v<const char, v<R>>, v<R>*>;
};  //struct throw_exception_arg_array_



}  //namespace detail


class exception_shared_string{
    public:
    explicit exception_shared_string(const char* const);
    exception_shared_string(const char*, std::size_t);
    template<typename S, typename = decltype((void)std::declval<const S&>().data(),(void)std::declval<const S&>().size())>
    explicit exception_shared_string(const S& str) 
    : exception_shared_string(str.data(), str.size()) {}

    exception_shared_string(const exception_shared_string&) noexcept;
    ~exception_shared_string();
    void operator=(const exception_shared_string&) = delete;

    const char* what() const noexcept;

    private:
    struct state;
    state* const state_;    
};


template<typename T>
class static_what_exception : public T{
    protected:
    struct StaticLifetimeTag{};

    public:
    using T::T;

    static_what_exception(StaticLifetimeTag, const char* msg) 
    :T(std::string{}), msg_{msg} {}

    const char* what() const noexcept override{
        return msg_ == nullptr ? T::what() : msg_;
    }

    private:
    const char* msg_ = nullptr;
};

}  //namespace hy

#endif  //HY_EXCEPTION_H_