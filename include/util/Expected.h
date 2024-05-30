#ifndef EXCEPTED_H_
#define EXCEPTED_H_

#include <exception>
#include <tuple>
#include <type_traits>
#include <utility>

#include "Preprocessor.h"
#include "Traits.hpp"

// Macro
// 用于生产异常消息常量字符串
// 等价于"HYX__LINE__",此为一个标记罢了
#define HY_EXPECTED_ID(X) HY_CONCATENATE(HY_CONCATENATE(HY, X), __LINE__)

// 和folly有区别，此实现少了一个","符号作为前缀，这更符合直觉
#define HY_REQUIRES_IMPL(...)                                                  \
  bool HY_EXPECTED_ID(                                                         \
      Requires) = false,                                                       \
     std::enable_if_t<                                                         \
         (HY_EXPECTED_ID(Requires) || static_cast<bool>(__VA_ARGS__)), int> =  \
         0

// 等价于", HY_REQUIRES_IMPL(__VA_ARGS__)"
// 问: 为什么是TRAILING而不是INSERT?
// 答: 默认实参需要排在最后面,
// 使用INSERT语义并不能保证用户会对靠后的模板形参提供默认实参
#define HY_REQUIRES_TRAILING(...) HY_REQUIRES_IMPL(__VA_ARGS__)

// 等价于"template <HY_REQUIRES_IMPL(__VA_ARGS__)>
#define HY_REQUIRES(...) template <HY_REQUIRES_IMPL(__VA_ARGS__)>

namespace hy {
namespace expected_detail {
namespace expected_detail_ExpectedHelper {
struct ExpectedHelper; // 声明
} // namespace expected_detail_ExpectedHelper
// 将expected_detail_ExpectedHelper::ExpectedHelper引入expected_detail命名空间
using expected_detail_ExpectedHelper::ExpectedHelper;
} // namespace expected_detail

namespace expected_detail {

/************struct ExpectedStorage及其附属工具************/
/***********************Begin-Tag*************************/
enum class StorageType { ePODStruct, ePODUnion, eUnion };

enum class Which : unsigned char { eEmpty, eValue, eError };

struct EmptyTag final {};
struct ValueTag final {};
struct ErrorTag final {};
/**************************End-Tag*************************/

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
  return StrategyConjunction_v<std::is_trivially_copyable, Value, Error>
             ? (sizeof(std::tuple<Value, Error>) <= sizeof(void *[2]) &&
                        StrategyConjunction_v<std::is_trivial, Value, Error>
                    ? StorageType::ePODStruct
                    : StorageType::eUnion)
             : StorageType::eUnion;
} // getStorageType()

/* 使用当前的机器的环境(long 和 int)去判断使用那种行为. 意义是什么? 不知道*/
/* 要么为t赋予新值(自动调用旧值的析构并对新值使用拷贝构造)*/
/* 要么对t原地重构(手动调用旧值析构并对t进行placement new)*/
/* 问: 此处有一个细节,只有当t是operator new构造时才能使用重构t?*/
/* 答:
 * 不一定,根据下文的使用可知,当对类成员使用时,亦可对t进行重构,因为其是栈内存,会自动释放*/
/* 目前还不是很懂其意义,主要是为了对Union使用*/
template <typename T, typename U>
auto doEmplaceAssign(int, T &t, U &&u) noexcept(noexcept(
    t = std::forward<U>(u))) -> decltype(void(t = std::forward<U>(u))) {
  t = std::forward<U>(u);
}

/*同上*/
template <class T, class U>
auto doEmplaceAssign(long, T &t,
                     U &&u) -> decltype(void(T(std::forward<U>(u)))) {
  auto addr = const_cast<void *>(static_cast<void const *>(std::addressof(t)));
  t.~T();
  ::new (addr) T(std::forward<U>(u));
}

/*同上*/
template <class T, class... Us>
auto doEmplaceAssign(int, T &t, Us &&...us) noexcept(
    noexcept(t = T(std::forward<Us>(us)...)))
    -> decltype(void(t = T(std::forward<Us>(us)...))) {
  t = T(std::forward<Us>(us)...);
}

/*同上*/
template <class T, class... Us>
auto doEmplaceAssign(long, T &t,
                     Us &&...us) -> decltype(void(T(std::forward<Us>(us)...))) {
  auto addr = const_cast<void *>(static_cast<void const *>(std::addressof(t)));
  t.~T();
  ::new (addr) T(std::forward<Us>(us)...);
}

/*此类用于存储异常的类型*/
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
  Which which_; // 选择标志位

  // 当Error能默认构造时才进行调用
  template <typename E = Error, typename = decltype(E{})>
  constexpr ExpectedStorage() noexcept(E{}) : error_{}, which_(Which::eError) {}

  // 此处Folly使用了自定义类型去默认构造ch_,但是添加了一系列编译器命令去抑制警告,主要是为了提升性能
  explicit constexpr ExpectedStorage(EmptyTag) noexcept
      : ch_{0}, which_{Which::eEmpty} {}

  // 使用空类ErrorTag作为判断当前构造函数构造的是那种错误类型的依据.
  // 问: 为什么使用新的类型struct ValueTag作为判断而不使用enum class Which?
  // 答: 因为要在初始化器列表进行初始化,只有使用类型去在形参列表进行重载判断.
  // 再对Value类型的成员进行初始化
  template <typename... Vs>
  explicit constexpr ExpectedStorage(ValueTag, Vs &&...vs) noexcept(
      noexcept(Value(std::forward<Vs>(vs)...)))
      : value_{std::forward<Vs>(vs)...}, which_{Which::eValue} {}

  // 使用空类ErrorTag作为判断当前构造函数构造的是那种错误类型的依据
  // 再对Error类型的成员进行初始化
  template <typename... Es>
  explicit constexpr ExpectedStorage(ErrorTag, Es &&...es) noexcept(
      noexcept(Error(std::forward<Es>(es)...)))
      : error_{std::forward<Es>(es)...}, which_{Which::eError} {}

  // 无需要变化的数据成员,清除什么?
  void clear() const noexcept {}

  // 对于完全构造的ExpectedStorage,其肯定是eValue或eError
  static constexpr bool uninitializedByException() noexcept { return false; }

  // 对现有的内容进行重新赋值,只有使用定制版的assign才能保证赋值正常
  // 对形参的实参类型有要求,不然会UB
  template <typename... Vs> void assignValue(Vs &&...vs) {
    expected_detail::doEmplaceAssign(0, value_, std::forward<Vs>(vs)...);
    which_ = Which::eValue;
  }

  // 同上
  template <typename... Es> void assignError(Es &&...es) {
    expected_detail::doEmplaceAssign(0, error_, std::forward<Es>(es)...);
    which_ = Which::eError;
  }

  /*智能选择重新拷贝分配的对象类型*/
  template <typename Other> void assign(Other &&that) {
    switch (that.which_) {
    case Which::eValue:
      this->assignValue(std::forward<Other>(that).value());
      break;
    case Which::eError:
      this->assignError(std::forward<Other>(that).error());
      break;
    case Which::eEmpty:
    default:
      this->clear();
      break;
    }
  }

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

/*struct ExpectedStorage调用示意图*/
// Constructor:
// 三种构造函数:eError、eValue、eEmpty三种(使用三种tag进行重载分发选择),默认使用eError
// assign: 三种重新拷贝/移动分配器针对{eError}||{eValue}||{完整对象}
// {eError}||{eValue}将调用doEmplaceAssign函数进行重新分配;

} // namespace expected_detail

/**
 * Unexpected - a helper type used to disambiguate the construction of
 * Expected objects in the error state.
 */
template <typename Error> class Unexpected final {
  /*Linit*/
  static_assert(!std::is_reference_v<Error>, "Error must not a reference!");

  /*Friend*/
  template <typename E> friend class Unexpected;
  template <typename V, typename E> friend class Expected;
  friend struct expected_detail::expected_detail_ExpectedHelper::ExpectedHelper;

public:
  /*Constructors*/
  Unexpected() = default;
  Unexpected(const Unexpected &) = default;
  Unexpected(Unexpected &&) = default;
  Unexpected &operator=(const Unexpected &) = default;
  Unexpected &operator=(Unexpected &&) = default;
  UNLIKELY constexpr Unexpected(const Error &err) : error_{err} {}
  UNLIKELY constexpr Unexpected(Error &&err) : error_{std::move(err)} {}
  template <typename Other,
            HY_REQUIRES_TRAILING(std::is_constructible_v<Error, Other &>)>
  constexpr Unexpected(Unexpected<Other> that) : error_{that.error()} {}

  /*Assignment*/
  template <typename Other,
            HY_REQUIRES_TRAILING(std::is_assignable_v<Error &, Other &&>)>
  Unexpected &operator=(const Unexpected<Other> &that) {
    error_ = that.error();
  }

  template <typename Other,
            HY_REQUIRES_TRAILING(std::is_assignable_v<Error &, Other &&>)>
  Unexpected &operator=(const Unexpected<Other> &&that) {
    error_ = std::move(that.error());
  }

  /*Observers*/
  Error &error() & noexcept { return error_; }
  const Error &error() const & noexcept { return error_; }
  Error &&error() && noexcept { return std::move(error_); }
  const Error &&error() const && noexcept { return std::move(error_); }

private:
  Error error_;
}; // class Unexpected

// 判断Unexpected<Error>是否相等
template <typename Error,
          HY_REQUIRES_TRAILING(hy::is_equality_comparable_v<Error>)>
inline bool operator==(const Unexpected<Error> &lhs,
                       const Unexpected<Error> &rhs) {
  return lhs.error() == rhs.error();
}
// 判断Unexpected<Error>是否不相等
template <typename Error,
          HY_REQUIRES_TRAILING(hy::is_equality_comparable_v<Error>)>
inline bool operator!=(const Unexpected<Error> &lhs,
                       const Unexpected<Error> &rhs) {
  /*复用上式*/
  return !(lhs == rhs);
}

// 最终构造的Unexpected的内在元素是无cv和无引用的,相当于构造器
template <typename Error>
[[nodiscard]] constexpr Unexpected<std::decay_t<Error>>
makeUnexpected(Error &&err) {
  return Unexpected<std::decay_t<Error>>{std::forward<Error>(err)};
}

template <class Error> class BadExpectedAccess; /// class BadExpectedAccess声明

template <> class BadExpectedAccess<void> : public std::exception {
public:
  BadExpectedAccess() noexcept =
      default; /// Folly在此处有explicit,但实际此explicit无作用
  BadExpectedAccess(const BadExpectedAccess &) noexcept {}
  BadExpectedAccess &operator=(const BadExpectedAccess &) noexcept {
    return *this;
  }

  char const *what() const noexcept override { return "bad expected access"; }
}; // class BadExpectedAccess<void>

template <typename Error>
class BadExpectedAccess : public BadExpectedAccess<void> {
public:
  explicit BadExpectedAccess(Error &&error)
      : error_{std::forward<Error>(error)} {}

  Error &error() & { return error_; }
  Error const &error() const & { return error_; }
  Error &&error() && { return static_cast<Error &&>(error_); }
  Error const &&error() const && { return static_cast<Error const &&>(error_); }

private:
  Error error_;
}; // class BadExpectedAccess

template <class Value, class Error> class Expected;

// 此处与Folly有区别,主要是模板形参顺序
template <typename Value, typename Error>
[[nodiscard]] constexpr Expected<std::decay_t<Error>, Value>
makeExpected(Error &&);

template <typename Expected>
using ExpectedValueType =
    typename std::remove_reference_t<Expected>::value_type;

template <class Expected>
using ExpectedErrorType =
    typename std::remove_reference_t<Expected>::error_type;

namespace expected_detail {
  

} //namespace expected_detail

} // namespace hy

#endif // EXCEPTED_H_