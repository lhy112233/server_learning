#ifndef HY_EXPECTED_HPP_
#define HY_EXPECTED_HPP_
#include <type_traits>
#include "Bad_expected_access.hpp"
#include "Unexpected.hpp"

namespace hy {

namespace detail {
template <typename Value, typename Error>
struct expected_base {
  /*Constructors*/
  template <typename Val = Value,
            std::enable_if_t<std::is_default_constructible_v<Val>>>
  constexpr expected_base() : val_{} {}
  constexpr expected_base(const expected_base& other) {
    if (has_val_) {
      val_ = other.val_;
    } else {
      unex_ = other.unex_;
    }
  }
  constexpr expected_base(expected_base&& other) noexcept(
      std::is_nothrow_move_constructible_v<Value> &&
      std::is_nothrow_move_constructible_v<Error>) {
    if (has_val_) {
      val_ = std::move(other.val_);
    } else {
      unex_ = std::move(other.unex_);
    }
  }

  bool has_val_;
  union {
    Value val_;
    Error unex_;
  };
};  //struct expected_base
}  //namespace detail

template <typename Value, typename Error>
class expected final : detail::expected_base<Value, Error> {
  /*friend*/
  template <typename V, typename E>
  friend struct detail::expected_base;
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
  constexpr expected(const expected& other) {}

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




/*Destructor*/
~expected() = default;



  /*Setters*/
  template< class... Args >
constexpr Value& emplace( Args&&... args ) noexcept{
    
}

};  //class expected

}  //namespace hy

#endif  ///HY_EXPECTED_HPP_