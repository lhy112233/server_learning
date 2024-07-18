#ifndef HY_BAD_EXPECTED_ACCESS_HPP_
#define HY_BAD_EXPECTED_ACCESS_HPP_
#include <algorithm>
#include <exception>
namespace hy {
/*Forward declear*/
template <typename E>
class bad_expected_access;

template <>
class bad_expected_access<void> : public std::exception {
 protected:
  const char* what() const noexcept(noexcept(std::exception::what())) {
    return std::exception::what();
  }
};

template <class E>
class bad_expected_access : public hy::bad_expected_access<void> {
 public:
  /*Constructors*/
  explicit bad_expected_access(E e) : error_{std::move(e)} {}
  /*Getters*/
  const E& error() const& noexcept { return error_; }
  E& error() & noexcept { return error_; }
  const E&& error() const&& noexcept { return std::move(error_); }
  E&& error() && noexcept { return std::move(error_); }

  const char* what() const noexcept override {
    return bad_expected_access<void>::what();
  }

 private:
  E error_;
};
}  //namespace hy

#endif  //HY_BAD_EXPECTED_ACCESS_HPP_