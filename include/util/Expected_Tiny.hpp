#ifndef HY_EXPECTED_HPP_
#define HY_EXPECTED_HPP_
#include <cstring>
#include <functional>
#include <memory>
#include <type_traits>
#include <utility>
#include "Bad_expected_access.hpp"
#include "Traits.hpp"
#include "Unexpected.hpp"
#include "Utility.h"

namespace hy {
/*Forward declear*/
template <typename V, typename E>
class expected;

/*details...*/
namespace details {
/*Helpers*/
template <typename T, typename = void>
struct is_unexpected_specialization : std::false_type {};

template <typename T>
struct is_unexpected_specialization<
    T, std::void_t<
           typename hy::remove_cvref_t<T>::error_type,
           std::enable_if_t<std::is_same_v<
               hy::remove_cvref_t<T>,
               hy::unexpected<typename hy::remove_cvref_t<T>::error_type>>>>>
    : std::true_type {};

template <typename T>
inline constexpr bool is_unexpected_specialization_v =
    is_unexpected_specialization<T>::value;

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
  /*1*/
  template <
      typename = std::enable_if_t<std::conjunction_v<
          std::is_default_constructible<V>, std::is_default_constructible<E>>>>
  ExpectedStorage() : has_val_(true), val_{} {}

  /*2*/
  template <typename = void, typename...>
  ExpectedStorage(const ExpectedStorage& rhs) = delete;

  template <
      typename Vt = V, typename Et = E,
      typename = std::enable_if_t<std::conjunction_v<
          std::is_copy_constructible<Vt>, std::is_copy_constructible<Et>>>>
  ExpectedStorage(const ExpectedStorage& rhs) : has_val_(rhs.has_value()) {
    if (has_value()) {
      hy::construct_at(std::addressof(val_), rhs.value());
    } else {
      hy::construct_at(std::addressof(unex_), rhs.error());
    }
  }

  /*3*/
  template <typename = void, typename...>
  ExpectedStorage(ExpectedStorage&& rhs) = delete;

  template <typename = std::enable_if_t<std::conjunction_v<
                std::is_move_constructible<V>, std::is_move_constructible<E>>>>
  ExpectedStorage(ExpectedStorage&& rhs) noexcept(
      noexcept(std::is_nothrow_move_constructible_v<V>&&
                   std::is_nothrow_move_constructible_v<E>))
      : has_val_(std::move(rhs.has_val_)) {
    if (has_value()) {
      hy::construct_at(std::addressof(val_), std::move(rhs.value()));
    } else {
      hy::construct_at(std::addressof(unex_), std::move(rhs.error()));
    }
  }

  /*4*/
  template <
      typename U, typename G,
      typename = std::enable_if_t<
          std::conjunction_v<std::is_constructible<V, const U&>,
                             std::is_constructible<E, const G&>> &&
          !std::disjunction_v<
              std::is_constructible<hy::unexpected<E>, hy::expected<U, G>&>,
              std::is_constructible<hy::unexpected<E>, hy::expected<U, G>>,
              std::is_constructible<hy::unexpected<E>,
                                    const hy::expected<U, G>&>,
              std::is_constructible<hy::unexpected<E>,
                                    const hy::expected<U, G>>> &&
          std::is_same_v<bool, std::remove_cv_t<V>>>,
      typename...>
  constexpr ExpectedStorage(const ExpectedStorage<U, G>& other)
      : has_val_(other.has_value()) {
    if (has_value()) {
      hy::construct_at(std::addressof(val_), std::forward<const U&>(*other));
    } else {
      hy::construct_at(std::addressof(unex_),
                       std::forward<const G&>(other.error()));
    }
  }

  template <
      typename U, typename G,
      typename = std::enable_if_t<
          std::conjunction_v<std::is_constructible<V, const U&>,
                             std::is_constructible<E, const G&>> &&
          !std::disjunction_v<
              std::is_constructible<hy::unexpected<E>, hy::expected<U, G>&>,
              std::is_constructible<hy::unexpected<E>, hy::expected<U, G>>,
              std::is_constructible<hy::unexpected<E>,
                                    const hy::expected<U, G>&>,
              std::is_constructible<hy::unexpected<E>,
                                    const hy::expected<U, G>>> &&
          !std::is_same_v<bool, std::remove_cv_t<V>> &&
          !std::disjunction_v<
              std::is_constructible<V, hy::expected<U, G>&>,
              std::is_constructible<V, hy::expected<U, G>>,
              std::is_constructible<V, const hy::expected<U, G>&>,
              std::is_constructible<V, const hy::expected<U, G>>,
              std::is_convertible<hy::expected<U, G>&, V>,
              std::is_convertible<hy::expected<U, G>, V>,
              std::is_convertible<const hy::expected<U, G>&, V>,
              std::is_convertible<const hy::expected<U, G>, V>>>>
  constexpr ExpectedStorage(const ExpectedStorage<U, G>& other)
      : has_val_(other.has_value()) {
    if (has_value()) {
      hy::construct_at(std::addressof(val_), std::forward<const U&>(*other));
    } else {
      hy::construct_at(std::addressof(unex_),
                       std::forward<const G&>(other.error()));
    }
  }

  /*5*/
  template <
      class U, class G,
      typename = std::enable_if_t<
          std::conjunction_v<std::is_constructible<V, U>,
                             std::is_constructible<E, G>> &&
          !std::disjunction_v<
              std::is_constructible<hy::unexpected<E>, hy::expected<U, G>&>,
              std::is_constructible<hy::unexpected<E>, hy::expected<U, G>>,
              std::is_constructible<hy::unexpected<E>,
                                    const hy::expected<U, G>&>,
              std::is_constructible<hy::unexpected<E>,
                                    const hy::expected<U, G>>> &&
          std::is_same_v<bool, std::remove_cv_t<V>>>,
      typename...>
  constexpr ExpectedStorage(ExpectedStorage<U, G>&& other)
      : has_val_(other.has_value()) {
    if (has_value()) {
      hy::construct_at(std::addressof(val_), std::forward<U>(*other));
    } else {
      hy::construct_at(std::addressof(unex_), std::forward<G>(other.error()));
    }
  }

  template <
      class U, class G,
      typename = std::enable_if_t<
          std::conjunction_v<std::is_constructible<V, U>,
                             std::is_constructible<E, G>> &&
          !std::disjunction_v<
              std::is_constructible<hy::unexpected<E>, hy::expected<U, G>&>,
              std::is_constructible<hy::unexpected<E>, hy::expected<U, G>>,
              std::is_constructible<hy::unexpected<E>,
                                    const hy::expected<U, G>&>,
              std::is_constructible<hy::unexpected<E>,
                                    const hy::expected<U, G>>> &&
          !std::is_same_v<bool, std::remove_cv_t<V>> &&
          !std::disjunction_v<
              std::is_constructible<V, hy::expected<U, G>&>,
              std::is_constructible<V, hy::expected<U, G>>,
              std::is_constructible<V, const hy::expected<U, G>&>,
              std::is_constructible<V, const hy::expected<U, G>>,
              std::is_convertible<hy::expected<U, G>&, V>,
              std::is_convertible<hy::expected<U, G>, V>,
              std::is_convertible<const hy::expected<U, G>&, V>,
              std::is_convertible<const hy::expected<U, G>, V>>>>
  constexpr ExpectedStorage(ExpectedStorage<U, G>&& other)
      : has_val_(other.has_value()) {
    if (has_value()) {
      hy::construct_at(std::addressof(val_), std::forward<U>(*other));
    } else {
      hy::construct_at(std::addressof(unex_), std::forward<G>(other.error()));
    }
  }

  /*6*/
  template <class U = V,
            typename = std::enable_if_t<
                !std::is_same_v<hy::remove_cvref_t<U>, std::in_place_t> &&
                !std::is_same_v<hy::expected<V, E>, hy::remove_cvref_t<U>> &&
                std::is_constructible_v<V, U> && std::is_convertible_v<U, V> &&
                !details::is_unexpected_specialization_v<U> /*&& */>,
            typename...>
  constexpr ExpectedStorage(U&& v) : has_val_(true), val_{std::forward<U>(v)} {}

  template <class U = V,
            typename = std::enable_if_t<
                !std::is_same_v<hy::remove_cvref_t<U>, std::in_place_t> &&
                !std::is_same_v<hy::expected<V, E>, hy::remove_cvref_t<U>> &&
                std::is_constructible_v<V, U> && !std::is_convertible_v<U, V>>>
  constexpr explicit ExpectedStorage(U&& v)
      : has_val_(true), val_{std::forward<U>(v)} {}

  /*7*/
  template <class G,
            typename = std::enable_if_t<std::is_constructible_v<E, const G&> &&
                                        std::is_convertible_v<const G&, E>>,
            typename...>
  constexpr ExpectedStorage(const hy::unexpected<G>& e)
      : has_val_(false), unex_{std::forward<const G&>(e.error())} {}

  template <class G,
            typename = std::enable_if_t<std::is_constructible_v<E, const G&> &&
                                        !std::is_convertible_v<const G&, E>>>
  constexpr explicit ExpectedStorage(const hy::unexpected<G>& e)
      : has_val_(false), unex_{std::forward<const G&>(e.error())} {}

  /*8*/
  template <class G,
            typename = std::enable_if_t<std::is_constructible_v<E, G> &&
                                        std::is_convertible_v<G, E>>,
            typename...>
  constexpr ExpectedStorage(hy::unexpected<G>&& e)
      : has_val_(false), unex_{std::forward<G>(e.error())} {}

  template <class G,
            typename = std::enable_if_t<std::is_constructible_v<E, G> &&
                                        !std::is_convertible_v<G, E>>>
  constexpr explicit ExpectedStorage(hy::unexpected<G>&& e)
      : has_val_(false), unex_{std::forward<G>(e.error())} {}

  /*9*/
  template <typename... Args,
            typename = std::enable_if_t<std::is_constructible_v<V, Args...>>>
  constexpr explicit ExpectedStorage(std::in_place_t, Args&&... args)
      : has_val_(true), val_{std::forward<Args>(args)...} {}

  /*10*/
  template <class U, class... Args,
            typename = std::enable_if_t<
                std::is_constructible_v<V, std::initializer_list<U>&, Args...>>>
  constexpr explicit ExpectedStorage(std::in_place_t,
                                     std::initializer_list<U> il,
                                     Args&&... args)
      : has_val_(true), val_{il, std::forward<Args>(args)...} {}

  /*11*/
  template <class... Args,
            typename = std::enable_if_t<std::is_constructible_v<E, Args...>>>
  constexpr explicit ExpectedStorage(hy::unexpect_t, Args&&... args)
      : has_val_(false), unex_{std::forward<Args>(args)...} {}

  /*12*/
  template <class U, class... Args,
            typename = std::enable_if_t<
                std::is_constructible_v<E, std::initializer_list<U>&, Args...>>>
  constexpr explicit ExpectedStorage(hy::unexpect_t,
                                     std::initializer_list<U> il,
                                     Args&&... args)
      : has_val_(false), unex_{il, std::forward<Args>(args)...} {}

  /*Assignment*/


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
      using std::swap;
      swap(val_, other.val_);
    } else if (!has_value() && !other.has_value()) {
      using std::swap;
      swap(unex_, other.unex_);
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
class ExpectedStorage<V, E, ExpectedStorageType::ePODUnion> {  //ePODUnion
  /*details*/
  template <class NewType, class OldType, class... Args>
  void reinit_expected(NewType& new_val, OldType& old_val, Args&&... args) {
    // 情况 1：“new_val” 的构造不会抛出：
    // 在销毁 “old_val” 后可以直接构造 “new_val”
    if constexpr (std::is_nothrow_constructible_v<NewType, Args...>) {
      std::destroy_at(std::addressof(old_val));
      hy::construct_at(std::addressof(new_val), std::forward<Args>(args)...);
    }
    // 情况 2：“new_val” 的移动构造不会抛出：
    // 先构造一个临时的 NewType 对象
    // （如果该构造抛出异常，那么 “old_val” 不会受到影响）
    else if constexpr (std::is_nothrow_move_constructible_v<NewType>) {
      NewType temp(std::forward<Args>(args)...);  // 可能会抛出异常
      std::destroy_at(std::addressof(old_val));
      hy::construct_at(std::addressof(new_val), std::move(temp));
    }
    // 情况 3：“new_val” 的构造可能会抛出：
    // 需要备份 “old_val” 以从异常中恢复
    else {
      OldType temp(std::move(old_val));  // 可能会抛出异常
      std::destroy_at(std::addressof(old_val));
      try {
        hy::construct_at(std::addressof(new_val),
                         std::forward<Args>(args)...);  // 可能会抛出异常
      } catch (...) {
        hy::construct_at(std::addressof(old_val), std::move(temp));
        throw;
      }
    }
  }

 public:
  /*Constructors*/
  /*1*/
  template <typename Vt = V,
            std::enable_if_t<std::is_default_constructible_v<Vt>>>
  constexpr ExpectedStorage() : has_val_{true}, val_{} {}

  /*2*/
  constexpr ExpectedStorage(const ExpectedStorage& rhs) = default;

  /*3*/

  ExpectedStorage(ExpectedStorage&& rhs) noexcept(
      noexcept(std::is_nothrow_move_constructible_v<V>&&
                   std::is_nothrow_move_constructible_v<E>)) = default;

  /*4*/

  /*10*/
  template <class U, class... Args, typename Vt = V,
            typename = std::enable_if_t<std::is_constructible_v<
                Vt, std::initializer_list<U>&, Args...>>>
  constexpr explicit ExpectedStorage(std::in_place_t,
                                     std::initializer_list<U> il,
                                     Args&&... args)
      : has_val_(true), val_{il, std::forward<Args>(args)...} {}

  /*11*/
  template <class... Args, typename Et = E,
            typename = std::enable_if_t<std::is_constructible_v<Et, Args...>>>
  constexpr explicit ExpectedStorage(hy::unexpect_t, Args&&... args)
      : has_val_{false}, unex_{std::forward<Args>(args)...} {}

  /*12*/
  template <class U, class... Args, typename Et = E,
            typename = std::enable_if_t<std::is_constructible_v<
                Et, std::initializer_list<U>&, Args...>>>
  constexpr explicit ExpectedStorage(hy::unexpect_t,
                                     std::initializer_list<U> il,
                                     Args&&... args)
      : has_val_(false), unex_{il, std::forward<Args>(args)...} {}

  /*Assignment*/
  constexpr ExpectedStorage& operator=(const ExpectedStorage& other) = default;
  constexpr ExpectedStorage& operator=(ExpectedStorage&& other) = default;

  template <class U = V,
            typename = std::enable_if_t<
                !std::is_same_v<ExpectedStorage, hy::remove_cvref_t<U>> &&
                std::conjunction_v<
                    std::is_constructible<V, U>, std::is_assignable<V&, U>,
                    std::disjunction<std::is_nothrow_constructible<V, U>,
                                     std::is_nothrow_move_constructible<V>,
                                     std::is_nothrow_move_constructible<E>>>>>
  constexpr ExpectedStorage& operator=(U&& v) {
    if (has_value()) {
      val_ = std::forward<U>(v);
    } else {
      reinit_expected(val_, unex_, std::forward<U>(v));
      has_val_ = false;
    }
    return *this;
  }

  template <
      class G,
      typename = std::enable_if_t<std::conjunction_v<
          std::is_constructible<E, const G&>, std::is_assignable<E&, const G&>,
          std::disjunction<std::is_nothrow_constructible<E, const G&>,
                           std::is_nothrow_move_constructible<V>,
                           std::is_nothrow_move_constructible<E>>>>>
  constexpr ExpectedStorage& operator=(const hy::unexpected<G>& e) {
    if (has_value()) {
      reinit_expected(val_, unex_, std::forward<const G&>(e.error()));
      has_val_ = false;
    } else {
      unex_ = std::forward<const G&>(e.error());
    }
    return *this;
  }

  template <class G,
            typename = std::enable_if_t<std::conjunction_v<
                std::is_constructible<E, G>, std::is_assignable<E&, G>,
                std::disjunction<std::is_nothrow_constructible<E, G>,
                                 std::is_nothrow_move_constructible<V>,
                                 std::is_nothrow_move_constructible<E>>>>>
  constexpr ExpectedStorage& operator=(hy::unexpected<G>&& e) {
    if (has_value()) {
      reinit_expected(val_, unex_, std::forward<G>(e.error()));
      has_val_ = false;
    } else {
      unex_ = std::forward<G>(e.error());
    }
    return *this;
  }

  /*Destory*/
  ~ExpectedStorage() = default;

  /*Obserbers*/
  constexpr const V* operator->() const noexcept {
    return std::addressof(val_);
  }

  constexpr V* operator->() noexcept { return std::addressof(val_); }

  constexpr const V& operator*() const& noexcept { return val_; }

  constexpr V& operator*() & noexcept { return val_; }

  constexpr const V&& operator*() const&& noexcept { return std::move(val_); }

  constexpr V&& operator*() && noexcept { return std::move(val_); }

  constexpr explicit operator bool() const noexcept { return has_value(); }

  constexpr bool has_value() const noexcept { return has_val_; }

  constexpr V& value() & {
    static_assert(std::is_copy_constructible_v<E>, "");
    if (!has_value()) {
      throw hy::bad_expected_access(std::as_const(error()));
    }
    return val_;
  }

  constexpr const V& value() const& {
    static_assert(std::is_copy_constructible_v<E>, "");
    if (!has_value()) {
      throw hy::bad_expected_access(std::as_const(error()));
    }
    return val_;
    ;
  }

  constexpr V&& value() && {
    static_assert(std::conjunction_v<
                      std::is_copy_constructible<E>,
                      std::is_constructible<E, decltype(std::move(error()))>>,
                  "");
    if (!has_value()) {
      throw hy::bad_expected_access(std::move(error()));
    }
    return std::move(val_);
  }

  constexpr const V&& value() const&& {
    static_assert(std::conjunction_v<
                      std::is_copy_constructible<E>,
                      std::is_constructible<E, decltype(std::move(error()))>>,
                  "");
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
  constexpr void swap(ExpectedStorage& other) noexcept {
    if constexpr (sizeof(V) > sizeof(E)) {
      using std::swap;
      swap(val_, other.val_);
      swap(has_val_, other.has_val_);
    } else {
      using std::swap;
      swap(unex_, other.unex_);
      swap(has_val_, other.has_val_);
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
class ExpectedStorage<V, E, ExpectedStorageType::ePODStruct> {  //ePODStruct
  /*details*/
  template <class NewType, class OldType, class... Args>
  void reinit_expected(NewType& new_val, OldType& old_val, Args&&... args) {
    // 情况 1：“new_val” 的构造不会抛出：
    // 在销毁 “old_val” 后可以直接构造 “new_val”
    if constexpr (std::is_nothrow_constructible_v<NewType, Args...>) {
      std::destroy_at(std::addressof(old_val));
      hy::construct_at(std::addressof(new_val), std::forward<Args>(args)...);
    }
    // 情况 2：“new_val” 的移动构造不会抛出：
    // 先构造一个临时的 NewType 对象
    // （如果该构造抛出异常，那么 “old_val” 不会受到影响）
    else if constexpr (std::is_nothrow_move_constructible_v<NewType>) {
      NewType temp(std::forward<Args>(args)...);  // 可能会抛出异常
      std::destroy_at(std::addressof(old_val));
      hy::construct_at(std::addressof(new_val), std::move(temp));
    }
    // 情况 3：“new_val” 的构造可能会抛出：
    // 需要备份 “old_val” 以从异常中恢复
    else {
      OldType temp(std::move(old_val));  // 可能会抛出异常
      std::destroy_at(std::addressof(old_val));
      try {
        hy::construct_at(std::addressof(new_val),
                         std::forward<Args>(args)...);  // 可能会抛出异常
      } catch (...) {
        hy::construct_at(std::addressof(old_val), std::move(temp));
        throw;
      }
    }
  }

 public:
  /*Constructors*/
  constexpr ExpectedStorage() : has_val_{true}, val_{} {}
  constexpr ExpectedStorage(const ExpectedStorage&) = default;
  constexpr ExpectedStorage(ExpectedStorage&&) noexcept(
      std::is_nothrow_move_constructible_v<V>&&
          std::is_nothrow_move_constructible_v<E>) = default;
  template <class U, class G,
            typename = std::enable_if_t<
                std::conjunction_v<std::is_constructible<V, const U&>,
                                   std::is_constructible<E, const G&>>>,
            typename = std::enable_if_t<std::is_same_v<V, bool> ||
                                        std::conjunction_v</*TODO*/>>>
  constexpr ExpectedStorage(const ExpectedStorage<U, G>& other) {
    if (other.has_value()) {
      has_val_ = true;
      hy::construct_at(std::addressof(val_), std::forward<const U&>(*other));
    } else {
      has_val_ = false;
      hy::construct_at(std::addressof(unex_),
                       std::forward<const G&>(other.error()));
    }
  }

  /*Assignment*/
  constexpr ExpectedStorage& operator=(const ExpectedStorage& other) = default;
  constexpr ExpectedStorage& operator=(ExpectedStorage&& other) noexcept =
      default;

  template <class U = V,
            typename = std::enable_if_t<
                !std::is_same_v<ExpectedStorage, hy::remove_cvref_t<U>> &&
                std::conjunction_v<
                    std::is_constructible<V, U>, std::is_assignable<V&, U>,
                    std::disjunction<std::is_nothrow_constructible<V, U>,
                                     std::is_nothrow_move_constructible<V>,
                                     std::is_nothrow_move_constructible<E>>>>>
  constexpr ExpectedStorage& operator=(U&& v) {
    if (has_value()) {
      val_ = std::forward<U>(v);
    } else {
      reinit_expected(val_, unex_, std::forward<U>(v));
      has_val_ = false;
    }
    return *this;
  }

  template <
      class G,
      typename = std::enable_if_t<std::conjunction_v<
          std::is_constructible<E, const G&>, std::is_assignable<E&, const G&>,
          std::disjunction<std::is_nothrow_constructible<E, const G&>,
                           std::is_nothrow_move_constructible<V>,
                           std::is_nothrow_move_constructible<E>>>>>
  constexpr ExpectedStorage& operator=(const hy::unexpected<G>& e) {
    if (has_value()) {
      reinit_expected(val_, unex_, std::forward<const G&>(e.error()));
      has_val_ = false;
    } else {
      unex_ = std::forward<const G&>(e.error());
    }
    return *this;
  }

  template <class G,
            typename = std::enable_if_t<std::conjunction_v<
                std::is_constructible<E, G>, std::is_assignable<E&, G>,
                std::disjunction<std::is_nothrow_constructible<E, G>,
                                 std::is_nothrow_move_constructible<V>,
                                 std::is_nothrow_move_constructible<E>>>>>
  constexpr ExpectedStorage& operator=(hy::unexpected<G>&& e) {
    if (has_value()) {
      reinit_expected(val_, unex_, std::forward<G>(e.error()));
      has_val_ = false;
    } else {
      unex_ = std::forward<G>(e.error());
    }
    return *this;
  }

  /*Destory*/
  ~ExpectedStorage() = default;

  constexpr const V* operator->() const noexcept {
    return std::addressof(val_);
  }

  constexpr V* operator->() noexcept { return std::addressof(val_); }

  constexpr const V& operator*() const& noexcept { return val_; }

  constexpr V& operator*() & noexcept { return val_; }

  constexpr const V&& operator*() const&& noexcept { return std::move(val_); }

  constexpr V&& operator*() && noexcept { return std::move(val_); }

  constexpr explicit operator bool() const noexcept { return has_value(); }

  constexpr bool has_value() const noexcept { return has_val_; }

  constexpr V& value() & {
    static_assert(std::is_copy_constructible_v<E>, "");
    if (!has_value()) {
      throw hy::bad_expected_access(std::as_const(error()));
    }
    return val_;
  }

  constexpr const V& value() const& {
    static_assert(std::is_copy_constructible_v<E>, "");
    if (!has_value()) {
      throw hy::bad_expected_access(std::as_const(error()));
    }
    return val_;
    ;
  }

  constexpr V&& value() && {
    static_assert(std::conjunction_v<
                      std::is_copy_constructible<E>,
                      std::is_constructible<E, decltype(std::move(error()))>>,
                  "");
    if (!has_value()) {
      throw hy::bad_expected_access(std::move(error()));
    }
    return std::move(val_);
  }

  constexpr const V&& value() const&& {
    static_assert(std::conjunction_v<
                      std::is_copy_constructible<E>,
                      std::is_constructible<E, decltype(std::move(error()))>>,
                  "");
    if (!has_value()) {
      throw hy::bad_expected_access(std::move(error()));
    }
    return std::move(val_);
  }

  constexpr const E& error() const& noexcept { return unex_; }

  constexpr E& error() & noexcept { return unex_; }

  constexpr const E&& error() const&& noexcept { return std::move(unex_); }

  constexpr E&& error() && noexcept { return std::move(unex_); }

  constexpr void swap(ExpectedStorage& other) noexcept {
    if constexpr (sizeof(ExpectedStorage) < 64) {
      char buf[sizeof(ExpectedStorage)];
      std::memcpy(buf, other, sizeof(ExpectedStorage));
      std::memcpy(std::addressof(other), this, sizeof(ExpectedStorage));
      std::memcpy(this, buf, sizeof(ExpectedStorage));
    } else {
      if (has_value() == other.has_value() && has_value()) {
        using std::swap;
        swap(val_, other.val_);
      } else if (has_value() == other.has_value() && !has_value()) {
        using std::swap;
        swap(unex_, other.unex_);
      } else {
        using std::swap;
        swap(val_, other.val_);
        swap(unex_, other.unex_);
        swap(has_val_, other.has_val_);
      }
    }
  }

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
  /*Type members*/
  using value_type = V;
  using error_type = E;
  using unexpected_type = hy::unexpected<E>;

  /*Member alias templates*/
  template <typename U>
  using rebind = hy::expected<U, error_type>;

  /*Constructors*/
  /*1*/
  template <typename Vt = V,
            typename = std::enable_if_t<std::is_default_constructible_v<Vt>>>
  expected() : details::ExpectedStorage<V, E>{} {}

  /*2*/
  template <
      typename Vt = V, typename Et = E,
      typename = std::enable_if_t<!std::conjunction_v<
          std::is_copy_constructible<Vt>, std::is_copy_constructible<Et>>>,
      typename...>
  expected(const expected& rhs) = delete;

  template <
      typename Vt = V, typename Et = E,
      typename = std::enable_if_t<std::conjunction_v<
          std::is_copy_constructible<Vt>, std::is_copy_constructible<Et>>>>
  expected(const expected& rhs) : details::ExpectedStorage<V, E>{rhs} {}

  /*3*/
  template <
      typename Vt = V, typename Et = E,
      typename = std::enable_if_t<!std::conjunction_v<
          std::is_move_constructible<Vt>, std::is_move_constructible<Et>>>,
      typename...>
  expected(expected&& rhs) = delete;

  template <
      typename Vt = V, typename Et = E,
      typename = std::enable_if_t<std::conjunction_v<
          std::is_move_constructible<Vt>, std::is_move_constructible<Et>>>>
  expected(expected&& rhs) noexcept(
      noexcept(std::is_nothrow_move_constructible_v<V>&&
                   std::is_nothrow_move_constructible_v<E>))
      : details::ExpectedStorage<V, E>{std::move(rhs)} {}

  /*4*/
  template <
      typename U, typename G, typename Vt = V,
      typename = std::enable_if_t<
          std::conjunction_v<std::is_constructible<V, const U&>,
                             std::is_constructible<E, const G&>> &&
          !std::disjunction_v<
              std::is_constructible<hy::unexpected<E>, hy::expected<U, G>&>,
              std::is_constructible<hy::unexpected<E>, hy::expected<U, G>>,
              std::is_constructible<hy::unexpected<E>,
                                    const hy::expected<U, G>&>,
              std::is_constructible<hy::unexpected<E>,
                                    const hy::expected<U, G>>> &&
          std::is_same_v<bool, std::remove_cv_t<Vt>>>,
      typename...>
  constexpr expected(const expected<U, G>& other)
      : details::ExpectedStorage<V, E>{other} {}

  template <
      typename U, typename G, typename Vt = V,
      typename = std::enable_if_t<
          std::conjunction_v<std::is_constructible<V, const U&>,
                             std::is_constructible<E, const G&>> &&
          !std::disjunction_v<
              std::is_constructible<hy::unexpected<E>, hy::expected<U, G>&>,
              std::is_constructible<hy::unexpected<E>, hy::expected<U, G>>,
              std::is_constructible<hy::unexpected<E>,
                                    const hy::expected<U, G>&>,
              std::is_constructible<hy::unexpected<E>,
                                    const hy::expected<U, G>>> &&
          !std::is_same_v<bool, std::remove_cv_t<Vt>> &&
          !std::disjunction_v<
              std::is_constructible<V, hy::expected<U, G>&>,
              std::is_constructible<V, hy::expected<U, G>>,
              std::is_constructible<V, const hy::expected<U, G>&>,
              std::is_constructible<V, const hy::expected<U, G>>,
              std::is_convertible<hy::expected<U, G>&, V>,
              std::is_convertible<hy::expected<U, G>, V>,
              std::is_convertible<const hy::expected<U, G>&, V>,
              std::is_convertible<const hy::expected<U, G>, V>>>>
  constexpr expected(const expected<U, G>& other)
      : details::ExpectedStorage<V, E>{other} {}

  /*5*/
  template <
      class U, class G, typename Vt = V,
      typename = std::enable_if_t<
          std::conjunction_v<std::is_constructible<V, U>,
                             std::is_constructible<E, G>> &&
          !std::disjunction_v<
              std::is_constructible<hy::unexpected<E>, hy::expected<U, G>&>,
              std::is_constructible<hy::unexpected<E>, hy::expected<U, G>>,
              std::is_constructible<hy::unexpected<E>,
                                    const hy::expected<U, G>&>,
              std::is_constructible<hy::unexpected<E>,
                                    const hy::expected<U, G>>> &&
          std::is_same_v<bool, std::remove_cv_t<Vt>>>,
      typename...>
  constexpr expected(expected<U, G>&& other)
      : details::ExpectedStorage<V, E>{std::move(other)} {}

  template <
      class U, class G, typename Vt = V,
      typename = std::enable_if_t<
          std::conjunction_v<std::is_constructible<V, U>,
                             std::is_constructible<E, G>> &&
          !std::disjunction_v<
              std::is_constructible<hy::unexpected<E>, hy::expected<U, G>&>,
              std::is_constructible<hy::unexpected<E>, hy::expected<U, G>>,
              std::is_constructible<hy::unexpected<E>,
                                    const hy::expected<U, G>&>,
              std::is_constructible<hy::unexpected<E>,
                                    const hy::expected<U, G>>> &&
          !std::is_same_v<bool, std::remove_cv_t<Vt>> &&
          !std::disjunction_v<
              std::is_constructible<V, hy::expected<U, G>&>,
              std::is_constructible<V, hy::expected<U, G>>,
              std::is_constructible<V, const hy::expected<U, G>&>,
              std::is_constructible<V, const hy::expected<U, G>>,
              std::is_convertible<hy::expected<U, G>&, V>,
              std::is_convertible<hy::expected<U, G>, V>,
              std::is_convertible<const hy::expected<U, G>&, V>,
              std::is_convertible<const hy::expected<U, G>, V>>>>
  constexpr expected(expected<U, G>&& other)
      : details::ExpectedStorage<V, E>{std::move(other)} {}

  /*6*/
  template <class U = V, typename Vt = V,
            typename = std::enable_if_t<
                !std::is_same_v<hy::remove_cvref_t<U>, std::in_place_t> &&
                !std::is_same_v<hy::expected<V, E>, hy::remove_cvref_t<U>> &&
                std::is_constructible_v<V, U> && std::is_convertible_v<U, V> &&
                !details::is_unexpected_specialization_v<U> /*&& */>,
            typename...>
  constexpr expected(U&& v)
      : details::ExpectedStorage<V, E>{std::forward<U>(v)} {}

  template <class U = V, typename Vt = V,
            typename = std::enable_if_t<
                !std::is_same_v<hy::remove_cvref_t<U>, std::in_place_t> &&
                !std::is_same_v<hy::expected<V, E>, hy::remove_cvref_t<U>> &&
                std::is_constructible_v<V, U> && !std::is_convertible_v<U, V> &&
                !details::is_unexpected_specialization_v<U> /*&&*/>>
  constexpr explicit expected(U&& v)
      : details::ExpectedStorage<V, E>{std::forward<U>(v)} {}

  /*7*/
  template <class G,
            typename = std::enable_if_t<std::is_constructible_v<E, const G&> &&
                                        std::is_convertible_v<const G&, E>>,
            typename...>
  constexpr expected(const hy::unexpected<G>& e)
      : details::ExpectedStorage<V, E>{e} {}

  template <class G,
            typename = std::enable_if_t<std::is_constructible_v<E, const G&> &&
                                        !std::is_convertible_v<const G&, E>>>
  constexpr explicit expected(const hy::unexpected<G>& e)
      : details::ExpectedStorage<V, E>{e} {}

  /*8*/
  template <class G,
            typename = std::enable_if_t<std::is_constructible_v<E, G> &&
                                        std::is_convertible_v<G, E>>,
            typename...>
  constexpr expected(hy::unexpected<G>&& e)
      : details::ExpectedStorage<V, E>{std::move(e)} {}

  template <class G,
            typename = std::enable_if_t<std::is_constructible_v<E, G> &&
                                        !std::is_convertible_v<G, E>>>
  constexpr explicit expected(hy::unexpected<G>&& e)
      : details::ExpectedStorage<V, E>{std::move(e)} {}

  /*9*/
  template <typename... Args,
            typename = std::enable_if_t<std::is_constructible_v<V, Args...>>>
  constexpr explicit expected(std::in_place_t, Args&&... args)
      : details::ExpectedStorage<V, E>{std::in_place,
                                       std::forward<Args>(args)...} {}

  /*10*/
  template <class U, class... Args,
            typename = std::enable_if_t<
                std::is_constructible_v<V, std::initializer_list<U>&, Args...>>>
  constexpr explicit expected(std::in_place_t, std::initializer_list<U> il,
                              Args&&... args)
      : details::ExpectedStorage<V, E>{std::in_place, il,
                                       std::forward<Args>(args)...} {}

  /*11*/
  template <class... Args,
            typename = std::enable_if_t<std::is_constructible_v<E, Args...>>>
  constexpr explicit expected(hy::unexpect_t, Args&&... args)
      : details::ExpectedStorage<V, E>{unexpect, std::forward<Args>(args)...} {}

  /*12*/
  template <class U, class... Args,
            typename = std::enable_if_t<
                std::is_constructible_v<E, std::initializer_list<U>&, Args...>>>
  constexpr explicit expected(hy::unexpect_t, std::initializer_list<U> il,
                              Args&&... args)
      : details::ExpectedStorage<V, E>{unexpect, il,
                                       std::forward<Args>(args)...} {}

  /*Destory*/
  ~expected() = default;

  /*Assignments*/
  constexpr expected& operator=(const expected& other) {
    base::operator=(other);
    return *this;
  }

  constexpr expected& operator=(expected&& other) noexcept(
      noexcept(base::operator=(std::move(other)))) {
    base::operator=(std::move(other));
    return *this;
  }

  template <class U = V>
  constexpr expected& operator=(U&& v) {
    base::operator=(std::move(v));
    return *this;
  }

  template <class G>
  constexpr expected& operator=(const hy::unexpected<G>& e) {
    base::operator=(e);
    return *this;
  }

  template <class G>
  constexpr expected& operator=(hy::unexpected<G>&& e) {
    base::operator=(std::move(e));
    return *this;
  }

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
  template <class F, typename Et = E,
            typename = std::enable_if_t<
                std::is_constructible_v<Et, decltype(std::declval<Et&>())>>>
  constexpr auto and_then(F&& f) & {
    static_assert(std::is_same_v<typename remove_cvref_t<std::invoke_result_t<
                                     F, decltype((value()))>>::error_type,
                                 E>,
                  "");
    using U = hy::remove_cvref_t<std::invoke_result_t<F, decltype((value()))>>;
    if (has_value()) {
      return std::invoke(std::forward<F>(f), value());
    } else {
      return U(hy::unexpect, error());
    }
  }

  template <class F, typename Et = E,
            typename = std::enable_if_t<std::is_constructible_v<
                Et, decltype(std::declval<const Et&>())>>>
  constexpr auto and_then(F&& f) const& {
    static_assert(std::is_same_v<typename remove_cvref_t<std::invoke_result_t<
                                     F, decltype((value()))>>::error_type,
                                 E>,
                  "");
    using U = hy::remove_cvref_t<std::invoke_result_t<F, decltype((value()))>>;
    if (has_value()) {
      return std::invoke(std::forward<F>(f), value());
    } else {
      return U(hy::unexpect, error());
    }
  }

  template <class F, typename Et = E,
            typename = std::enable_if_t<std::is_constructible_v<
                Et, decltype(std::move(std::declval<Et&&>()))>>>
  constexpr auto and_then(F&& f) && {
    static_assert(
        std::is_same_v<typename remove_cvref_t<std::invoke_result_t<
                           F, decltype(std::move(value()))>>::error_type,
                       E>,
        "");
    using U = hy::remove_cvref_t<
        std::invoke_result_t<F, decltype(std::move(value()))>>;
    if (has_value()) {
      return std::invoke(std::forward<F>(f), std::move(value()));
    } else {
      return U(hy::unexpect, std::move(error()));
    }
  }

  template <typename F, typename Et = E,
            typename = std::enable_if_t<std::is_constructible_v<
                Et, decltype(std::move(std::declval<const Et&&>()))>>>
  constexpr auto and_then(F&& f) const&& {
    static_assert(
        std::is_same_v<typename remove_cvref_t<std::invoke_result_t<
                           F, decltype(std::move(value()))>>::error_type,
                       E>,
        "");
    using U = hy::remove_cvref_t<
        std::invoke_result_t<F, decltype(std::move(value()))>>;
    if (has_value()) {
      return std::invoke(std::forward<F>(f), std::move(value()));
    } else {
      return U(hy::unexpect, std::move(error()));
    }
  }

  template <typename F, typename Et = E,
            typename = std::enable_if_t<
                std::is_constructible_v<Et, decltype(std::declval<Et&>())>>>
  constexpr auto transform(F&& f) & {
    using U = std::remove_cv_t<std::invoke_result_t<F, decltype((value()))>>;
    if (has_value()) {
      if constexpr (std::is_void_v<U>) {
        std::invoke(std::forward<F>(f), value());
        return hy::expected<U, E>();
      } else {
        return hy::expected<U, E>(std::in_place,
                                  std::invoke(std::forward<F>(f), value()));
      }
    } else {
      return hy::expected<U, E>(hy::unexpect, error());
    }
  }

  template <typename F, typename Et = E,
            typename = std::enable_if_t<std::is_constructible_v<
                Et, decltype(std::declval<const Et&>())>>>
  constexpr auto transform(F&& f) const& {
    using U = std::remove_cv_t<std::invoke_result_t<F, decltype((value()))>>;
    if (has_value()) {
      if constexpr (std::is_void_v<U>) {
        std::invoke(std::forward<F>(f), value());
        return hy::expected<U, E>();
      } else {
        return hy::expected<U, E>(std::in_place,
                                  std::invoke(std::forward<F>(f), value()));
      }
    } else {
      return hy::expected<U, E>(hy::unexpect, error());
    }
  }

  template <typename F, typename Et = E,
            typename = std::enable_if_t<std::is_constructible_v<
                Et, decltype(std::move(std::declval<Et&&>()))>>>
  constexpr auto transform(F&& f) && {
    using U =
        std::remove_cv_t<std::invoke_result_t<F, decltype(std::move(value()))>>;
    if (has_value()) {
      if constexpr (std::is_void_v<U>) {
        std::invoke(std::forward<F>(f), std::move(value()));
        return hy::expected<U, E>();
      } else {
        return hy::expected<U, E>(
            std::in_place, std::invoke(std::forward<F>(f), std::move(value())));
      }
    } else {
      return hy::expected<U, E>(hy::unexpect, std::move(error()));
    }
  }

  template <typename F, typename Et = E,
            typename = std::enable_if_t<std::is_constructible_v<
                E, decltype(std::move(std::declval<const E&&>()))>>>
  constexpr auto transform(F&& f) const&& {
    using U =
        std::remove_cv_t<std::invoke_result_t<F, decltype(std::move(value()))>>;
    if (has_value()) {
      if constexpr (std::is_void_v<U>) {
        std::invoke(std::forward<F>(f), std::move(value()));
        return hy::expected<U, E>();
      } else {
        return hy::expected<U, E>(
            std::in_place, std::invoke(std::forward<F>(f), std::move(value())));
      }
    } else {
      return hy::expected<U, E>(hy::unexpect, std::move(error()));
    }
  }

  template <typename F, typename Vt = V,
            typename = std::enable_if_t<
                std::is_constructible_v<V, decltype((std::declval<Vt&>()))>>>
  constexpr auto or_else(F&& f) & {
    using G = hy::remove_cvref_t<std::invoke_result_t<F, decltype(error())>>;
    static_assert(std::is_same_v<typename G::value_type, V>, "");
    if (has_value()) {
      return G(std::in_place, value());
    } else {
      return std::invoke(std::forward<F>(f), error());
    }
  }

  template <class F, typename Vt = V,
            typename = std::enable_if_t<
                std::is_constructible_v<V, decltype((std::declval<Vt&>()))>>>
  constexpr auto or_else(F&& f) const& {
    using G = hy::remove_cvref_t<std::invoke_result_t<F, decltype(error())>>;
    static_assert(std::is_same_v<typename G::value_type, V>, "");
    if (has_value()) {
      return G(std::in_place, value());
    } else {
      return std::invoke(std::forward<F>(f), error());
    }
  }

  template <class F, typename Vt = V,
            typename = std::enable_if_t<std::is_constructible_v<
                V, decltype(std::move(std::declval<Vt&>()))>>>
  constexpr auto or_else(F&& f) && {
    using G = hy::remove_cvref_t<
        std::invoke_result_t<F, decltype(std::move(error()))>>;
    static_assert(std::is_same_v<typename G::value_type, V>, "");
    if (has_value()) {
      return G(std::in_place, std::move(value()));
    } else {
      return std::invoke(std::forward<F>(f), std::move(error()));
    }
  }

  template <typename F, typename Vt = V,
            typename = std::enable_if_t<std::is_constructible_v<
                V, decltype(std::move(std::declval<Vt&>()))>>>
  constexpr auto or_else(F&& f) const&& {
    using G = hy::remove_cvref_t<
        std::invoke_result_t<F, decltype(std::move(error()))>>;
    static_assert(std::is_same_v<typename G::value_type, V>, "");
    if (has_value()) {
      return G(std::in_place, std::move(value()));
    } else {
      return std::invoke(std::forward<F>(f), std::move(error()));
    }
  }

  template <typename F, typename Vt = V,
            typename = std::enable_if_t<std::is_constructible_v<
                Vt, std::add_lvalue_reference_t<std::remove_cv_t<Vt>>>>>
  constexpr auto transform_error(F&& f) & {
    using G = std::remove_cv_t<std::invoke_result_t<F, decltype(error())>>;
    if (has_value()) {
      return hy::expected<V, G>(std::in_place, value());
    } else {
      return hy::expected<V, G>(hy::unexpect,
                                std::invoke(std::forward<F>(f), error()));
    }
  }

  template <typename F, typename Vt = V,
            typename = std::enable_if_t<std::is_constructible_v<
                Vt, std::add_lvalue_reference_t<std::remove_cv_t<Vt>>>>>
  constexpr auto transform_error(F&& f) const& {
    using G = std::remove_cv_t<std::invoke_result_t<F, decltype(error())>>;
    if (has_value()) {
      return hy::expected<V, G>(std::in_place, value());
    } else {
      return hy::expected<V, G>(hy::unexpect,
                                std::invoke(std::forward<F>(f), error()));
    }
  }

  template <typename F, typename Vt = V,
            typename = std::enable_if_t<std::is_constructible_v<
                Vt, std::add_rvalue_reference_t<std::remove_cv_t<Vt>>>>>
  constexpr auto transform_error(F&& f) && {
    using G =
        std::remove_cv_t<std::invoke_result_t<F, decltype(std::move(error()))>>;
    if (has_value()) {
      return hy::expected<V, G>(std::in_place, std::move(value()));
    } else {
      return hy::expected<V, G>(
          hy::unexpect, std::invoke(std::forward<F>(f), std::move(error())));
    }
  }

  template <typename F, typename Vt = V,
            typename = std::enable_if_t<std::is_constructible_v<
                Vt, std::add_rvalue_reference_t<std::remove_cv_t<Vt>>>>>
  constexpr auto transform_error(F&& f) const&& {
    using G =
        std::remove_cv_t<std::invoke_result_t<F, decltype(std::move(error()))>>;
    if (has_value()) {
      return hy::expected<V, G>(std::in_place, std::move(value()));
    } else {
      return hy::expected<V, G>(
          hy::unexpect, std::invoke(std::forward<F>(f), std::move(error())));
    }
  }

  /*Modify*/
  template <typename... Args, typename = std::enable_if_t<
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

  template <typename U, typename... Args,
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

  template <typename Vt = V, typename Et = E,
            typename = std::enable_if_t<
                std::conjunction_v<std::is_swappable<Vt>, std::is_swappable<Et>,
                                   std::is_move_constructible<Vt>,
                                   std::is_move_constructible<Et>> &&
                std::disjunction_v<std::is_nothrow_move_constructible<Vt>,
                                   std::is_nothrow_move_constructible<Et>>>>
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
