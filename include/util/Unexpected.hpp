#ifndef HY_UNEXPECTED_HPP_
#define HY_UNEXPECTED_HPP_
#include <initializer_list>
#include <type_traits>
#include <utility>
#include "Traits.hpp"

namespace hy {
/**
 * Unexpected - a helper type used to disambiguate the construction of
 * expected objects in the error state.
 */
template <typename E>
class unexpected final {
  /*static_assert*/
  static_assert(!std::conjunction_v<std::is_const<E>, std::is_volatile<E>,
                                    std::is_array<E>> &&
                    std::is_object_v<E>,
                "Error must no cv and no object ,no array!");

 public:
  constexpr unexpected(const unexpected&) = default;
  constexpr unexpected(unexpected&&) = default;
  template <class Err = E,
            typename = std::enable_if_t<
                !std::is_same_v<hy::remove_cvref_t<Err>, unexpected> &&
                !std::is_same_v<hy::remove_cvref_t<Err>, std::in_place_t> &&
                std::is_constructible_v<E, Err>>>
  constexpr explicit unexpected(Err&& e) : err_(std::forward<Err>(e)) {}

  template <class... Args,
            typename = std::enable_if_t<std::is_constructible_v<E, Args...>>>
  constexpr explicit unexpected(std::in_place_t, Args&&... args)
      : err_(std::forward<Args>(args)...) {}

  template <class U, class... Args,
            typename = std::enable_if_t<
                std::is_constructible_v<E, std::initializer_list<U>&, Args...>>>
  constexpr explicit unexpected(std::in_place_t, std::initializer_list<U> il,
                                Args&&... args)
      : err_(il, std::forward<Args>(args)...) {}

  constexpr const E& error() const& noexcept { return err_; }

  constexpr E& error() & noexcept { return err_; }

  constexpr const E&& error() const&& noexcept { return std::move(err_); }

  constexpr E&& error() && noexcept { return std::move(err_); }

  constexpr void swap(unexpected& other) noexcept(
      std::is_nothrow_swappable_v<E>) {
    static_assert(std::is_swappable_v<E>, "E is disable swapable");
    using std::swap;
    swap(error(), other.error());
  }

  /*friends*/
  template <class E2>
  friend constexpr bool operator==(unexpected& x, hy::unexpected<E2>& y) {
    static_assert(is_equality_comparable_v<E&>, "E is disable equality compareable");
    return x.error() == y.error();
  }

  template <typename = std::enable_if_t<std::is_swappable_v<E>>>
  friend constexpr auto swap(unexpected& x,
                             unexpected& y) noexcept(noexcept(x.swap(y)))
      -> void {
        static_assert(std::is_swappable_v<E>, "E is disable swapable");
    x.swap(y);
  }

 private:
  E err_;
};

struct unexpect_t { explicit unexpect_t() = default; };

inline constexpr hy::unexpect_t unexpect{};

}  // namespace hy

#endif  //HY_UNEXPECTED_HPP_