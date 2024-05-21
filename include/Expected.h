#ifndef EXCEPTED_H_
#define EXCEPTED_H_
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <stdexcept>
#include <tuple>
#include <type_traits>
#include <utility>

#include "Preprocessor.h"

// Macro
// 用于生产异常消息常量字符串
// 等价于"HYX__LINE"
#define HY_EXPECTED_ID(X) HY_CONCATENATE(HY_CONCATENATE(HY, X), __LINE__)

//
#define HY_REQUIRES_IMPL(...)                                                  \
  bool HY_EXPECTED_ID(Requires) = false,                                       \
       std::enable_if_t<(HY_EXPECTED_ID(Requires) ||                           \
                         static_cast<bool>(__VA_ARGS__))>,                     \
       int > = 0;

namespace hy {
namespace expected_detail {
namespace expected_detail_ExpectedHelper {
struct ExpectedHelper {
  // template<typename V,typename E>
  // static void assume_empty(){}
};
} // namespace expected_detail_ExpectedHelper

enum class StorageType { ePODStruct, ePODUnion, eUnion };

enum class Which : unsigned char { eEmpty, eValue, eError };

struct EmptyTag {};

/**
 * @brief
 * 如果Value或Error其中一个不是可平凡拷贝的，那么返回eUnion,
 * 否则进入下一个判断:
 * 若Value+Error类型的对象大小
 * 小于等于两个指针的大小并且Value和Error是平凡布局，那么返回ePODStruct,否则返回ePODUnion.
 *
 * @tparam Value
 * @tparam Error
 * @return constexpr StorageType 字面量类型
 */
template <typename Value, typename Error>
constexpr StorageType getStorageType() {
  return std::conjunction_v<std::is_trivially_copyable<Value>,
                            std::is_trivially_copyable<Error>>
             ? (sizeof(std::tuple<Value, Error>) <= sizeof(void *[2]) &&
                        std::conjunction_v<std::is_trivial<Value>,
                                           std::is_trivial<Error>>
                    ? StorageType::ePODStruct
                    : StorageType::ePODUnion)
             : StorageType::eUnion;
} // getStorageType()

template <typename Value, typename Error,
          StorageType =
              expected_detail::getStorageType<Value, Error>()> // ePODUnion
struct ExpectedStorage {
  using Value_type = Value;
  using error_type = Error;
  union {
    Value value_;
    Error error_;
    char ch_;
  }; // union
  Which which_;

  template <typename E = Error,
            typename = std::enable_if_t<std::is_default_constructible_v<E>>>
  constexpr ExpectedStorage() noexcept(E{}) : error_{}, which_(Which::eError) {}

  explicit constexpr ExpectedStorage(EmptyTag) noexcept
      : ch_{}, which_{Which::eEmpty} {}

  // 以下很简单,判断是否为左值或右值或常量，从而调用性能最佳的函数
  Value &value() & { return value_; }
  const Value &value() const & { return value_; }
  Value &&value() && { return std::move(value_); }
  const Value &&value() const && {
    return std::move(value_);
  } // 没什么用的，只是为了类型完整性
  Error &error() & { return error_; }
  const Error &error() const & { return error_; }
  Error &&error() && { return std::move(error_); }
  const Error &&error() const && {
    return std::move(error_);
  } // 没什么用的，只是为了类型完整性
}; // struct ExpectedStorage

} // namespace expected_detail

template <typename Error> class Unexpected final {
  template <typename E> friend class Unexpected;
  template <typename V, typename E> friend class Expected;
  friend struct expected_detail::ExpectedHelper;

  Error error_;

public:
  Unexpected() = default;
  Unexpected(const Unexpected &) = default;
  Unexpected(Unexpected &&) = default;
  Unexpected &operator=(const Unexpected &) = default;
  Unexpected &operator=(Unexpected &&) = default;
  unlikely constexpr Unexpected(const Error &err) : error_{err} {}
  unlikely constexpr Unexpected(Error &&err) : error_{std::move(err)} {}

  Error &error() & noexcept { return error_; }
  const Error &error() const & noexcept { return error_; }
  Error &&error() && noexcept { return std::move(error_); }
  const Error &&error() const && noexcept { return std::move(error_); }

}; // class Unexpected

// template<typename Value,typename Error>
// class Expected final :expected_detail::Expected

} // namespace hy

#endif // EXCEPTED_H_