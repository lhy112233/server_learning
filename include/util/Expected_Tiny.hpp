#ifndef HY_EXPECTED_HPP_
#define HY_EXPECTED_HPP_
#include <memory>
#include <type_traits>
#include <utility>
#include "Bad_expected_access.hpp"
#include "Unexpected.hpp"

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
      throw std::bad_expected_access(std::as_const(error()));
    }
    return val_;
  }

  constexpr const V& value() const& {
    static_assert(std::is_copy_constructible_v<E>,
                  "V is unable copy construct");
    if (!has_value()) {
      throw std::bad_expected_access(std::as_const(error()));
    }
    return val_;
  }

  constexpr V&& value() && {
    static_assert(std::conjunction_v<
                      std::is_copy_constructible<E>,
                      std::is_constructible<E, decltype(std::move(error()))>>,
                  "V is unable move construct");
    if (!has_value()) {
      throw std::bad_expected_access(std::move(error()));
    }
    return std::move(val_);
  }

  constexpr const V&& value() const&& {
    static_assert(std::conjunction_v<
                      std::is_copy_constructible<E>,
                      std::is_constructible<E, decltype(std::move(error()))>>,
                  "V is unable move construct");
    if (!has_value()) {
      throw std::bad_expected_access(std::move(error()));
    }
    return std::move(val_);
  }

  constexpr const E& error() const& noexcept { return unex_; }

  constexpr E& error() & noexcept { return unex_; }

  constexpr const E&& error() const&& noexcept { return std::move(unex_); }

  constexpr E&& error() && noexcept { return std::move(unex_); }

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

  /*Modify*/
  template <class... Args, typename = std::enable_if_t<
                               std::is_nothrow_constructible_v<V, Args...>>>
  constexpr V& emplace(Args&&... args) noexcept {
    if (has_value()) {
      ~val_;
    } else {
      ~unex_;
    }
    return *std::construct_at(std::addressof(val_),
                              std::forward<Args>(args)...);
  }

  template <class U, class... Args,
            typename = std::enable_if_t<std::is_nothrow_constructible_v<
                V, std::initializer_list<U>&, Args...>>>
  constexpr V& emplace(std::initializer_list<U> il, Args&&... args) noexcept {
    if (has_value()) {
      ~val_;
    } else {
      ~unex_;
    }
    return *std::construct_at(std::addressof(val_), il,
                              std::forward<Args>(args)...);
  }

 private:
  bool has_val_;
  union {
    V val_;
    E unex_;
  };
};

}  // namespace details

template <typename V, typename E>
class expected final : public details::ExpectedStorage<V, E> {
 public:
};

}  //namespace hy

#endif  ///HY_EXPECTED_HPP_