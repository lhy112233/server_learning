#ifndef HY_EXPECTED_HPP_
#define HY_EXPECTED_HPP_
#include <memory>
#include <type_traits>
#include <utility>
#include "Bad_expected_access.hpp"
#include "Unexpected.hpp"

namespace hy {

template <typename Value, typename Error>
class expected final {
  /*friend*/
  template <typename V, typename E>
  friend class expected;

  /*static_assert*/
  static_assert(std::is_destructible_v<Value>, "Value must be destrucy");
  static_assert(std::is_destructible_v<Error>, "Error must be destrucy");

 public:
  /*Member types*/
  using value_type = Value;
  using error_type = Error;
  using unexpected_type = hy::unexpected<Error>;

  template <typename U>
  using rebind = hy::expected<U, error_type>;

  /*Constructors*/
  constexpr expected() = default;
  constexpr expected(const expected& other) : has_val_(other.has_value()) {
    if (has_value()) {
      value() = other.value();
    } else {
      error() = other.error();
    }
  }

  constexpr expected(const Value& val) : val_(val), has_val_(true) {}

  constexpr expected(const Error& err) : unex_(err), has_val_(false) {}

  /*Observers*/
  constexpr const Value* operator->() const noexcept {
    return std::addressof(this->val_);
  }

  constexpr Value* operator->() noexcept { return std::addressof(this->val_); }

  constexpr const Value& operator*() const& noexcept { return this->val_; }

  constexpr Value& operator*() & noexcept { return this->val_; }

  constexpr const Value&& operator*() const&& noexcept {
    return std::move(this->val_);
  }

  constexpr Value&& operator*() && noexcept { return std::move(this->val_); }

  constexpr explicit operator bool() const noexcept { return this->has_val_; }

  constexpr bool has_value() const noexcept { return this->has_val_; }

  constexpr Value& value() & {
    static_assert(std::is_copy_constructible_v<Error>,
                  "Error is disable to copy constructible");
    if (!has_value()) {
      throw hy::bad_expected_access(std::as_const(error()));
    }
    return this->val_;
  }

  constexpr const Value& value() const& {
    static_assert(std::is_copy_constructible_v<Error>,
                  "Error is disable to copy constructible");
    if (!has_value()) {
      throw hy::bad_expected_access(std::as_const(error()));
    }
    return this->val_;
  }

  constexpr Value&& value() && {
    static_assert(
        std::conjunction_v<
            std::is_copy_constructible<Error>,
            std::is_constructible<Error, decltype(std::move(error()))>>,
        "Error does not meet the requirements");
    if (!has_value()) {
      throw hy::bad_expected_access(std::move(error()));
    }
    return std::move(this->val_);
  }

  constexpr const Value&& value() const&& {
    static_assert(
        std::conjunction_v<
            std::is_copy_constructible<Error>,
            std::is_constructible<Error, decltype(std::move(error()))>>,
        "Error does not meet the requirements");
    if (!has_value()) {
      throw hy::bad_expected_access(std::move(error()));
    }
    return std::move(this->val_);
  }

  constexpr const Error& error() const& noexcept { return this->unex_; }

  constexpr Error& error() & noexcept { return this->unex_; }

  constexpr const Error&& error() const&& noexcept {
    return std::move(this->unex_);
  }

  constexpr Error&& error() && noexcept { return std::move(this->unex_); }

  template <class U>
  constexpr Value value_or(U&& default_value) const& {
    static_assert(std::conjunction_v<std::is_copy_constructible<Value>,
                                     std::is_convertible<U, Value>>,
                  "value_or's param type is unsuitable!");
    return has_value() ? **this
                       : static_cast<Value>(std::forward<U>(default_value));
  }

  template <class U>
  constexpr Value value_or(U&& default_value) && {
    static_assert(std::conjunction_v<std::is_move_constructible<Value>,
                                     std::is_convertible<U, Value>>,
                  "value_or's param type is unsuitable!");
    return has_value() ? std::move(**this)
                       : static_cast<Value>(std::forward<U>(default_value));
  }

  template <class G = Error>
  constexpr Error error_or(G&& default_value) const& {
    static_assert(std::conjunction_v<std::is_copy_constructible<Error>,
                                     std::is_convertible<G, Error>>,
                  "error_or's param type is unsuitable!");
    return has_value() ? std::forward<G>(default_value) : error();
  }

  template <class G = Error>
  constexpr Error error_or(G&& default_value) && {
    static_assert(std::conjunction_v<std::is_move_constructible<Error>,
                                     std::is_convertible<G, Error>>,
                  "error_or's param type is unsuitable!");
    return has_value() ? std::forward<G>(default_value) : std::move(error());
  }

  template <class F>
  constexpr auto and_then(F&& f) const& {
    if constexpr (has_value()) {
      return std::forward<F>(f)(value());
    } else {
      return hy::expected<Value, Error>{error()};
    }
  }

  template <class F>
  constexpr auto and_then(F&& f) && {
    if constexpr (has_value()) {
      return std::forward<F>(f)(value());
    } else {
      return hy::expected<Value, Error>{std::move(error())};
    }
  }

  template <class F>
  constexpr auto and_then(F&& f) const&& {
    if constexpr (has_value()) {
      return std::forward<F>(f)(value());
    } else {
      return hy::expected<Value, Error>{error()};
    }
  }

  /*Setters*/
  template <class... Args, typename = std::enable_if_t<
                               std::is_nothrow_constructible_v<Value, Args...>>>
  constexpr Value& emplace(Args&&... args) noexcept {}

  template <class U, class... Args>
  constexpr Value& emplace(std::initializer_list<U> il,
                           Args&&... args) noexcept {}

  constexpr void swap(expected& other) noexcept(
      std::is_nothrow_move_constructible_v<Value>&& std::is_nothrow_swappable_v<
          Value>&& std::is_nothrow_move_constructible_v<Error>&&
          std::is_nothrow_swappable_v<Error>) {
    if (has_value() && other.has_val()) {
      using std::swap;
      swap(value(), other.value());
    } else if (has_value() && !other.has_value()) {
      /*TODO*/
    } else if (!has_value() && has_value()) {
      other.swap(*this);
    } else {
      using std::swap;
      swap(error(), other.error());
    }
  }

  /*friend*/
  template <class T2, class E2,
            typename = std::enable_if_t<!std::is_void_v<T2>>>
  friend constexpr bool operator==(const expected& x,
                                   const expected<T2, E2>& y) {
    if (x.has_value()) {
      return x.value() == y.value();
    } else {
      return x.error() == y.error();
    }
  }

  template <class T2>
  friend constexpr bool operator==(const expected& x, const T2& val) {
    return x.value() == val;
  }

  template <class E2>
  friend constexpr bool operator==(const expected& x, const unexpected<E2>& e) {
    return x.error() == e.error();
  }

  friend constexpr void swap(expected& lhs,
                             expected& rhs) noexcept(noexcept(lhs.swap(rhs))) {
    lhs.swap(rhs);
  }

 private:
  bool has_val_;
  Value val_;
  Error unex_;

};  //class expected

}  //namespace hy

#endif  ///HY_EXPECTED_HPP_