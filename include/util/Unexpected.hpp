#ifndef HY_UNEXPECTED_HPP_
#define HY_UNEXPECTED_HPP_
#include <initializer_list>
#include <type_traits>
#include <utility>
#include "Traits.hpp"

namespace hy {
template <typename Error>
class unexpected final {
  /*static_assert*/
  static_assert(
      !std::conjunction_v<std::is_const<Error>, std::is_volatile<Error>,
                          std::is_array<Error>> &&
          std::is_object_v<Error>,
      "Error must no cv and no object ,no array!");
  /*friends*/
  template <class E2>
  friend constexpr bool operator==(unexpected& x, hy::unexpected<E2>& y) {
    return x.error() == y.error();
  }

  template <typename E>
  friend constexpr auto swap(unexpected& x, unexpected& y) noexcept(noexcept(
      x.swap(y))) -> decltype(std::enable_if_t<std::is_swappable_v<E>>{}) {
    x.swap(y);
  }

 public:
  /*Constructors*/
  constexpr unexpected(const unexpected&) = default;
  constexpr unexpected(unexpected&&) = default;

  template <class Err = Error,
            typename = std::enable_if_t<
                !std::is_same_v<hy::remove_cvref_t<Err>, unexpected> &&
                !std::is_same_v<hy::remove_cvref_t<Err>, std::in_place_t> &&
                std::is_constructible_v<Error, Err>>>
  constexpr explicit unexpected(Err&& e) : error_{std::forward<Err>(e)} {}

  template <class... Args, typename = std::enable_if_t<
                               std::is_constructible_v<Error, Args...>>>
  constexpr explicit unexpected(std::in_place_t, Args&&... args)
      : error_{std::forward<Args>(args)...} {}

  template <class U, class... Args,
            typename = std::enable_if_t<std::is_constructible_v<
                Error, std::initializer_list<U>&, Args...>>>
  constexpr explicit unexpected(std::in_place_t, std::initializer_list<U> il,
                                Args&&... args)
      : error_{il, std::forward<Args>(args)...} {}

  constexpr const Error& error() const& noexcept { return error_; }
  constexpr Error& error() & noexcept { return error_; }
  constexpr const Error&& error() const&& noexcept { return std::move(error_); }
  constexpr Error&& error() && noexcept { return std::move(error_); }

  constexpr void swap(unexpected& other) noexcept(
      std::is_nothrow_swappable_v<Error>) {
    using std::swap;
    swap(error(), other.error());
  }

 private:
  Error error_;
};  //class unexpected

/*Non-member functions*/

}  // namespace hy

#endif  //HY_UNEXPECTED_HPP_