#ifndef HY_EXPECTED_HPP_
#define HY_EXPECTED_HPP_
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include "Bad_expected_access.hpp"
#include "Traits.hpp"
#include "Unexpected.hpp"
#include "Utility.h"

namespace hy {
/*details...*/
namespace details {
enum class ExpectedStorageType {
  ePODStruct,
  ePODUnion,
  eUnion,
};

template <typename V, typename E>
inline constexpr ExpectedStorageType getExpectedStorageType() noexcept {
  return std::conjunction_v<std::is_pod<V>, std::is_pod<E>>
             ? (2 * sizeof(void*) > (sizeof(V) + sizeof(E))
                    ? ExpectedStorageType::ePODUnion
                    : ExpectedStorageType::ePODStruct)
             : ExpectedStorageType::eUnion;
}

template <typename V, typename E,
          ExpectedStorageType = getExpectedStorageType<V, E>()>  //eUnion
class ExpectedStorage {
 public:
  /*Constructors*/
  template <typename = std::enable_if_t<std::is_default_constructible_v<V>>>
  constexpr ExpectedStorage() : has_val_(true), val_{} {}

  template <typename = std::enable_if_t<!std::disjunction_v<
                std::is_copy_constructible<V>, std::is_copy_constructible<E>>>>
  constexpr ExpectedStorage(const ExpectedStorage& other) = delete;

  template <typename is_copyable = std::enable_if_t<std::conjunction_v<
                std::is_copy_constructible<V>, std::is_copy_constructible<E>>>,
            typename is_trivially = std::enable_if_t<
                std::conjunction_v<std::is_trivially_copy_constructible<V>,
                                   std::is_trivially_copy_constructible<E>>>>
  constexpr ExpectedStorage(const ExpectedStorage& other)
      : has_val_(other.has_value()) {
    if (has_value()) {
      val_ = other.value();
    } else {
      unex_ = other.error();
    }
  }

  template <typename is_copyable = std::enable_if_t<std::conjunction_v<
                std::is_copy_constructible<V>, std::is_copy_constructible<E>>>,
            typename is_trivially = std::enable_if_t<
                !std::conjunction_v<std::is_trivially_copy_constructible<V>,
                                    std::is_trivially_copy_constructible<E>>>,
            typename...>
  constexpr ExpectedStorage(const ExpectedStorage& other)
      : has_val_(other.has_value()) {
    if (has_value()) {
      val_ = other.value();
    } else {
      unex_ = other.error();
    }
  }

  /*Destory*/
  ~ExpectedStorage() {
    if (has_value()) {
      ~val_;
    } else {
      ~unex_;
    }
  }

  /*Observers*/
  constexpr const V* operator->() const noexcept {
    return std::addressof(val_);
  }

  constexpr V* operator->() noexcept { return std::addressof(val_); }

  constexpr const V& operator*() const& noexcept { return val_; }

  constexpr V& operator*() & noexcept { return val_; }

  constexpr const V&& operator*() const&& noexcept { return std::move(val_); }

  constexpr V&& operator*() && noexcept { return std::move(val_); }

  constexpr explicit operator bool() const noexcept { return has_val_; }

  constexpr bool has_value() const noexcept { return has_val_; }

  constexpr V& value() & {
    static_assert(std::is_copy_constructible_v<E>,
                  "V is unable copy construct");
    if (!has_value()) {
      throw hy::bad_expected_access(std::as_const(error()));
    }
    return val_;
  }

  constexpr const V& value() const& {
    static_assert(std::is_copy_constructible_v<E>,
                  "V is unable copy construct");
    if (!has_value()) {
      throw hy::bad_expected_access(std::as_const(error()));
    }
    return val_;
  }

  constexpr V&& value() && {
    static_assert(std::conjunction_v<
                      std::is_copy_constructible<E>,
                      std::is_constructible<E, decltype(std::move(error()))>>,
                  "V is unable move construct");
    if (!has_value()) {
      throw hy::bad_expected_access(std::move(error()));
    }
    return std::move(val_);
  }

  constexpr const V&& value() const&& {
    static_assert(std::conjunction_v<
                      std::is_copy_constructible<E>,
                      std::is_constructible<E, decltype(std::move(error()))>>,
                  "V is unable move construct");
    if (!has_value()) {
      throw hy::bad_expected_access(std::move(error()));
    }
    return std::move(val_);
  }

  constexpr const E& error() const& noexcept { return unex_; }

  constexpr E& error() & noexcept { return unex_; }

  constexpr const E&& error() const&& noexcept { return std::move(unex_); }

  constexpr E&& error() && noexcept { return std::move(unex_); }

  /*Swap*/
  template <typename = std::enable_if_t<
                std::conjunction_v<std::is_swappable<V>, std::is_swappable<E>,
                                   std::is_move_constructible<V>,
                                   std::is_move_constructible<E>> &&
                std::disjunction_v<std::is_nothrow_move_constructible<V>,
                                   std::is_nothrow_move_constructible<E>>>>
  void swap(ExpectedStorage& other) noexcept(
      std::is_nothrow_move_constructible_v<V>&& std::is_nothrow_swappable_v<V>&&
          std::is_nothrow_move_constructible_v<E>&&
              std::is_nothrow_swappable_v<E>) {
    if (has_value() && other.has_value()) {

    } else if (!has_value() && !other.has_value()) {
      using std::swap;
      swap(unex_, other._);
    } else if (!has_value() && other.has_value()) {
      other.swap(*this);
    } else {
      // 情况 1：非预期值的移动构造不会抛出：
      // 在 “other.val” 构造失败的情况下会复原 “other.unex”
      if constexpr (std::is_nothrow_move_constructible_v<E>) {
        E temp(std::move(other.unex));
        std::destroy_at(std::addressof(other.unex));
        try {
          hy::construct_at(std::addressof(other.val),
                           std::move(val_));  // 可能会抛出异常
          std::destroy_at(std::addressof(val_));
          hy::construct_at(std::addressof(unex_), std::move(temp));
        } catch (...) {
          hy::construct_at(std::addressof(other.unex), std::move(temp));
          throw;
        }
      }
      // 情况 2：预期值的移动构造不会抛出：
      // 在 “this->unex” 构造失败的情况下会复原 “this->val”
      else {
        V temp(std::move(val_));
        std::destroy_at(std::addressof(val_));
        try {
          hy::construct_at(std::addressof(unex_),
                           std::move(other.unex));  // 可能会抛出异常
          std::destroy_at(std::addressof(other.unex));
          hy::construct_at(std::addressof(other.val), std::move(temp));
        } catch (...) {
          hy::construct_at(std::addressof(val_), std::move(temp));
          throw;
        }
      }
      has_val_ = false;
      other.has_val_ = true;
    }
  }

 private:
  bool has_val_;
  union {
    V val_;
    E unex_;
  };
};

template <typename V, typename E>
class ExpectedStorage<V, E, ExpectedStorageType::ePODUnion> {
 public:
 /*Constructors*/
  constexpr ExpectedStorage() : has_val_{true}, val_{} {}
  constexpr ExpectedStorage(const ExpectedStorage& other) = default;
  constexpr ExpectedStorage(ExpectedStorage&& other) noexcept(
      std::is_nothrow_move_constructible_v<V>&&
          std::is_nothrow_move_constructible_v<E>) = default;

  
  /*Assignment*/
  constexpr ExpectedStorage& operator=( const ExpectedStorage& other ) = default;
  constexpr ExpectedStorage& operator=( ExpectedStorage&& other ) = default;


  /*Destory*/
  ~ExpectedStorage() = default;

  /*Obserbers*/
  constexpr const V* operator->() const noexcept{
    return std::addressof(val_);
  }

 private:
  bool has_val_;
  union {
    V val_;
    E unex_;
  };
};

template <typename V, typename E>
class ExpectedStorage<V, E, ExpectedStorageType::ePODStruct> {
 public:
 private:
  bool has_val_;
  V val_;
  E unex_;
};

}  // namespace details

template <typename V, typename E>
class expected final : public details::ExpectedStorage<V, E> {
  using base = typename details::ExpectedStorage<V, E>;

 public:
  /*Destory*/
  ~expected() = default;

  /*Obervers*/
  constexpr const V* operator->() const noexcept { return base::operator->(); }

  constexpr V* operator->() noexcept { return base::operator->(); }

  constexpr const V& operator*() const& noexcept { return base::operator*(); }

  constexpr V& operator*() & noexcept { return base::operator*(); }

  constexpr const V&& operator*() const&& noexcept {
    return std::move(base::operator*());
  }

  constexpr V&& operator*() && noexcept { return std::move(base::operator*()); }

  constexpr explicit operator bool() const noexcept {
    return base::operator bool();
  }

  constexpr bool has_value() const noexcept { return base::has_value(); }

  constexpr V& value() & {
    static_assert(std::is_copy_constructible_v<E>, "");
    if (!has_value()) {
      throw hy::bad_expected_access(std::as_const(error()));
    }
    return base::value();
  }

  constexpr const V& value() const& {
    static_assert(std::is_copy_constructible_v<E>, "");
    if (!has_value()) {
      throw hy::bad_expected_access(std::as_const(error()));
    }
    return base::value();
  }

  constexpr V&& value() && {
    static_assert(std::conjunction_v<
                      std::is_copy_constructible<E>,
                      std::is_constructible<E, decltype(std::move(error()))>>,
                  "");
    if (!has_value()) {
      throw hy::bad_expected_access(std::move(error()));
    }
    return std::move(base::value());
  }

  constexpr const V&& value() const&& {
    static_assert(std::conjunction_v<
                      std::is_copy_constructible<E>,
                      std::is_constructible<E, decltype(std::move(error()))>>,
                  "");
    if (!has_value()) {
      throw hy::bad_expected_access(std::move(error()));
    }
    return std::move(base::value());
  }

  constexpr const E& error() const& noexcept { return base::error(); }

  constexpr E& error() & noexcept { return base::error(); }

  constexpr const E&& error() const&& noexcept { return std::move(error()); }

  constexpr E&& error() && noexcept { return std::move(error()); }

  template <class U>
  constexpr V value_or(U&& default_value) const& {
    static_assert(std::conjunction_v<std::is_copy_constructible<V>,
                                     std::is_convertible<U, V>>,
                  "");
    return has_value() ? **this
                       : static_cast<V>(std::forward<U>(default_value));
  }

  template <class U>
  constexpr V value_or(U&& default_value) && {
    static_assert(std::conjunction_v<std::is_move_constructible<V>,
                                     std::is_convertible<U, V>>,
                  "");
    return has_value() ? std::move(**this)
                       : static_cast<V>(std::forward<U>(default_value));
  }

  template <class G = E>
  constexpr E error_or(G&& default_value) const& {
    static_assert(std::conjunction_v<std::is_copy_constructible<E>,
                                     std::is_convertible<G, E>>,
                  "");
    return has_value() ? std::forward<G>(default_value) : error();
  }

  template <class G = E>
  constexpr E error_or(G&& default_value) && {
    static_assert(std::conjunction_v<std::is_move_constructible<E>,
                                     std::is_convertible<G, E>>,
                  "");
    return has_value() ? std::forward<G>(default_value) : std::move(error());
  }

  /*sengial operator*/
  template <class F, typename = std::enable_if_t<std::is_constructible_v<
                         E, decltype(std::declval<E&>())>>>
  constexpr auto and_then(F&& f) & {
    static_assert(std::is_same_v<typename remove_cvref_t<std::invoke_result_t<
                                     F, decltype((value()))>>::error_type,
                                 E>,
                  "");
    if (has_value()) {
      return std::invoke(std::forward<F>(f), value());
    } else {
      return U(hy::unexpect, error());
    }
  }

  template <class F, typename = std::enable_if_t<std::is_constructible_v<
                         E, decltype(std::declval<const E&>())>>>
  constexpr auto and_then(F&& f) const& {
    static_assert(std::is_same_v<typename remove_cvref_t<std::invoke_result_t<
                                     F, decltype((value()))>>::error_type,
                                 E>,
                  "");
    if (has_value()) {
      return std::invoke(std::forward<F>(f), value());
    } else {
      return U(hy::unexpect, error());
    }
  }

  template <class F, typename = std::enable_if_t<std::is_constructible_v<
                         E, decltype(std::move(std::declval<E&&>()))>>>
  constexpr auto and_then(F&& f) && {
    static_assert(
        std::is_same_v<typename remove_cvref_t<std::invoke_result_t<
                           F, decltype(std::move(value()))>>::error_type,
                       E>,
        "");
    if (has_value()) {
      std::invoke(std::forward<F>(f), std::move(value()));
    } else {
      return U(hy::unexpect, std::move(error()));
    }
  }

  template <class F, typename = std::enable_if_t<std::is_constructible_v<
                         E, decltype(std::move(std::declval<const E&&>()))>>>
  constexpr auto and_then(F&& f) const&& {
    static_assert(
        std::is_same_v<typename remove_cvref_t<std::invoke_result_t<
                           F, decltype(std::move(value()))>>::error_type,
                       E>,
        "");
    if (has_value()) {
      std::invoke(std::forward<F>(f), std::move(value()));
    } else {
      return U(hy::unexpect, std::move(error()));
    }
  }

  template <class F, typename = std::enable_if_t<std::is_constructible_v<
                         E, decltype(std::declval<E&>())>>>
  constexpr auto transform(F&& f) & {}

  template <class F, typename = std::enable_if_t<std::is_constructible_v<
                         E, decltype(std::declval<const E&>())>>>
  constexpr auto transform(F&& f) const& {}

  template <class F, typename = std::enable_if_t<std::is_constructible_v<
                         E, decltype(std::move(std::declval<E&&>()))>>>
  constexpr auto transform(F&& f) && {}

  template <class F, typename = std::enable_if_t<std::is_constructible_v<
                         E, decltype(std::move(std::declval<const E&&>()))>>>
  constexpr auto transform(F&& f) const&& {}

  template <class F, typename = std::enable_if_t<std::is_constructible_v<
                         V, std::add_lvalue_reference_t<std::remove_cv_t<
                                decltype(std::declval<V>())>>>>>
  constexpr auto or_else(F&& f) & {}

  template <class F>
  constexpr auto or_else(F&& f) const& {}

  template <class F>
  constexpr auto or_else(F&& f) && {}

  template <class F>
  constexpr auto or_else(F&& f) const&& {}

  template <class F, typename = std::enable_if_t<std::is_constructible_v<
                         V, std::add_lvalue_reference_t<std::remove_cv_t<V>>>>>
  constexpr auto transform_error(F&& f) & {
    /*TODO*/
  }

  template <class F, typename = std::enable_if_t<std::is_constructible_v<
                         V, std::add_lvalue_reference_t<std::remove_cv_t<V>>>>>
  constexpr auto transform_error(F&& f) const& {
    /*TODO*/
  }

  template <class F, typename = std::enable_if_t<std::is_constructible_v<
                         V, std::add_rvalue_reference_t<std::remove_cv_t<V>>>>>
  constexpr auto transform_error(F&& f) && {
    /*TODO*/
  }

  template <class F, typename = std::enable_if_t<std::is_constructible_v<
                         V, std::add_rvalue_reference_t<std::remove_cv_t<V>>>>>
  constexpr auto transform_error(F&& f) const&& {
    /*TODO*/
  }

  /*Modify*/
  template <class... Args, typename = std::enable_if_t<
                               std::is_nothrow_constructible_v<V, Args...>>>
  constexpr V& emplace(Args&&... args) noexcept {
    if (has_value()) {
      ~value();
    } else {
      ~error();
    }
    return *hy::construct_at(std::addressof(value()),
                             std::forward<Args>(args)...);
  }

  template <class U, class... Args,
            typename = std::enable_if_t<std::is_nothrow_constructible_v<
                V, std::initializer_list<U>&, Args...>>>
  constexpr V& emplace(std::initializer_list<U> il, Args&&... args) noexcept {
    if (has_value()) {
      ~value();
    } else {
      ~error();
    }
    return *hy::construct_at(std::addressof(value()), il,
                             std::forward<Args>(args)...);
  }

  template <typename = std::enable_if_t<
                std::conjunction_v<std::is_swappable<V>, std::is_swappable<E>,
                                   std::is_move_constructible<V>,
                                   std::is_move_constructible<E>> &&
                std::disjunction_v<std::is_nothrow_move_constructible<V>,
                                   std::is_nothrow_move_constructible<E>>>>
  constexpr void swap(expected& other) noexcept(
      noexcept(base::swap(other.base))) {
    base::swap(other.base);
  }

  /*Friends*/
  template <class T2, class E2,
            typename = std::enable_if_t<!std::is_void_v<T2>>>
  friend constexpr bool operator==(const expected& x,
                                   const expected<T2, E2>& y) {
    static_assert(
        hy::is_equality_comparable_v<decltype(x.value()), decltype(y.value())>,
        "x.value() and y.value() cannot be compared as equal");
    static_assert(
        hy::is_equality_comparable_v<decltype(x.error()), decltype(y.error())>,
        "x.error() and y.error() cannot be compared as equal");
    static_assert(
        std::is_convertible_v<decltype(x.value() == y.value()), bool>,
        "The comparison result cannot be converted to bool type -- x.value() "
        "== y.value()");
    static_assert(
        std::is_convertible_v<decltype(x.error() == y.error()), bool>,
        "The comparison result cannot be converted to bool type -- x.error() "
        "== y.error()");

    return x.has_value() ? (y.has_value() && *x == *y)
                         : (!y.has_value() && x.error() == y.error());
  }

  template <class T2>
  friend constexpr bool operator==(const expected& x, const T2& val) {
    static_assert(
        hy::is_equality_comparable_v<decltype(x.value()), decltype(val)>,
        "x.value() and val cannot be compared as equal");
    static_assert(
        std::is_convertible_v<std::decay_t<decltype(x.value() == val)>, bool>,
        "The comparison result cannot be converted to bool type -- x.value() "
        "== val");
    return x.has_value() && static_cast<bool>(*x == val);
  }

  template <class E2>
  friend constexpr bool operator==(const expected& x, const unexpected<E2>& e) {
    static_assert(
        hy::is_equality_comparable_v<decltype(x.error()), decltype(e.error())>,
        "x.error() and e.error() cannot be compared as equal -- x.error() == "
        "e.error()");
    static_assert(
        std::is_convertible_v<
            std::remove_reference_t<decltype(x.error() == e.error())>, bool>,
        "The comparison result cannot be converted to bool type");
    return !x.has_value() && static_cast<bool>(x.error() == e.error());
  }

  friend constexpr auto swap(expected& lhs,
                             expected& rhs) noexcept(noexcept(lhs.swap(rhs)))
      -> decltype(lhs.swap(rhs)) {
    lhs.swap(rhs);
  }
};

}  //namespace hy

#endif  ///HY_EXPECTED_HPP_
