#ifndef HY_EXCEPTED_FOLLY_H_
#define HY_EXCEPTED_FOLLY_H_

#include <exception>
#include <tuple>
#include <type_traits>
#include <utility>

#include <cassert>
#include "Preprocessor.h"
#include "Traits.hpp"
#include "Bad_expected_access.hpp"
#include "Unexpected.hpp"

// Macro
// 用于生产异常消息常量字符串
// 等价于"HYX__LINE__",此为一个标记罢了
#define HY_EXPECTED_ID(X) HY_CONCATENATE(HY_CONCATENATE(HY, X), __LINE__)

// 和folly有区别，此实现少了一个","符号作为前缀，这更符合直觉
#define HY_REQUIRES_IMPL(...)                                                 \
  bool HY_EXPECTED_ID(                                                        \
      Requires) = false,                                                      \
     std::enable_if_t<                                                        \
         (HY_EXPECTED_ID(Requires) || static_cast<bool>(__VA_ARGS__)), int> = \
         0

// 等价于", HY_REQUIRES_IMPL(__VA_ARGS__)"
// 问: 为什么是TRAILING而不是INSERT?
// 答: 默认实参需要排在最后面,
// 使用INSERT语义并不能保证用户会对靠后的模板形参提供默认实参
#define HY_REQUIRES_TRAILING(...) HY_REQUIRES_IMPL(__VA_ARGS__)

// 等价于"template <HY_REQUIRES_IMPL(__VA_ARGS__)>
#define HY_REQUIRES(...) template <HY_REQUIRES_IMPL(__VA_ARGS__)>

namespace hy {

// Details...
namespace expected_detail {
namespace expected_detail_ExpectedHelper {
struct ExpectedHelper;  // 声明
}  // namespace expected_detail_ExpectedHelper
// 将expected_detail_ExpectedHelper::ExpectedHelper引入expected_detail命名空间
using expected_detail_ExpectedHelper::ExpectedHelper;
}  // namespace expected_detail

namespace expected_detail {

/**
 * namesapce expected_detail主要用于处理Expected的内部实现细节，
 * 重点类是模板类struct
 * ExpectedStorage,此模板类一共有三种细分实例化类型,用于处理保存数据的不同，
 * 三种类型由"enum class StorageType控制模板实参从而控制实例化。
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

template <typename Value, typename Error, typename = void>
struct Promise;

template <typename Value, typename Error>
struct PromiseReturn;

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
 * @details Value、Error is not trivially_copyable ==> StorageType::eUnion;
 *          Value、Error is trivially_copyable ==> Two possibilities ==>{
 *  {==> (1)、sizeof(std::tuple<Value, Error>) 小于等于 两个指针的大小 ==>
 * StorageType::ePODStruct;
 *  {==> (2)、sizeof(std::tuple<Value, Error>) 大于等于 两个指针的大小 ==>
 * StorageType::ePODUnion;
 */
template <typename Value, typename Error>
constexpr StorageType getStorageType() {
  return StrategyConjunction_v<std::is_trivially_copyable, Value, Error>
             ? (sizeof(std::tuple<Value, Error>) <= sizeof(void* [2]) &&
                        StrategyConjunction_v<std::is_trivial, Value, Error>
                    ? StorageType::ePODStruct
                    : StorageType::ePODUnion)
             : StorageType::eUnion;
}  // getStorageType()

/* 使用int和long作为函数参数的意义是对不能使用拷贝构造的情况使用第二种方法
   当然了,优先匹配int作为函数参数的情况*/
/* 要么为t赋予新值(自动调用旧值的析构并对新值使用拷贝构造)*/
/* 要么对t原地重构(手动调用旧值析构并对t进行placement new)*/
/* 问: 此处有一个细节,只有当t是operator new构造时才能使用重构t?*/
/* 答:
 * 不一定,根据下文的使用可知,当对类成员使用时,亦可对t进行重构,因为其是栈内存,会自动释放*/
/* 目前还不是很懂其意义,主要是为了对Union使用*/
/* 此处拥有一个细节,使用返回值的方法去进行SFINAE*/

/*下面两个函数能够被可变参数的版本给替代掉,想象不出其存在的意义,可能是写多了吧!!!*/
// template <typename T, typename U>
// auto doEmplaceAssign(int, T &t, U &&u) noexcept(noexcept(
//     t = std::forward<U>(u))) -> decltype(void(t = std::forward<U>(u))) {
//   t = std::forward<U>(u);
// }

// /*同上*/
// template <typename T, typename U>
// auto doEmplaceAssign(long, T &t,
//                      U &&u) -> decltype(void(T(std::forward<U>(u)))) {
//   auto addr = const_cast<void *>(static_cast<const void
//   *>(std::addressof(t))); t.~T();
//   ::new (addr) T(std::forward<U>(u));
// }

/*细节中的细节,当拷贝赋值运算符没有定义时,才会去考虑placement new的版本.
  注:(但placement new的版本也要拥有能够使用T(std::forward<Us>(us)...)的能力)*/
/*同上*/
template <typename T, typename... Us>
auto doEmplaceAssign(int, T& t, Us&&... us) noexcept(
    noexcept(t = T(std::forward<Us>(us)...)))
    -> decltype(void(t = T(std::forward<Us>(us)...))) {
  t = T(std::forward<Us>(us)...);
}

/*同上*/
template <typename T, typename... Us>
auto doEmplaceAssign(long, T& t, Us&&... us)
    -> decltype(void(T(std::forward<Us>(us)...))) {
  auto addr = const_cast<void*>(static_cast<const void*>(std::addressof(t)));
  t.~T();
  ::new (addr) T(std::forward<Us>(us)...);
}

/*****************ExpectedStorage Begin************************/

/*此类用于存储异常的类型*/
template <typename Value, typename Error,
          StorageType = getStorageType<Value, Error>()>  // ePODUnion
struct ExpectedStorage {
  using Value_type = Value;
  using error_type = Error;
  union {
    Value value_;
    Error error_;
    char ch_;
  };             // union
  Which which_;  // 选择标志位

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
  explicit constexpr ExpectedStorage(ValueTag, Vs&&... vs) noexcept(
      noexcept(Value(std::forward<Vs>(vs)...)))
      : value_{std::forward<Vs>(vs)...}, which_{Which::eValue} {}

  // 使用空类ErrorTag作为判断当前构造函数构造的是那种错误类型的依据
  // 再对Error类型的成员进行初始化
  template <typename... Es>
  explicit constexpr ExpectedStorage(ErrorTag, Es&&... es) noexcept(
      noexcept(Error(std::forward<Es>(es)...)))
      : error_{std::forward<Es>(es)...}, which_{Which::eError} {}

  // 无需要变化的数据成员,清除什么?
  void clear() const noexcept {}

  // 对于完全构造的ExpectedStorage,其肯定是eValue或eError
  static constexpr bool uninitializedByException() noexcept { return false; }

  // 对现有的内容进行重新赋值,只有使用定制版的assign才能保证赋值正常
  // 对形参的实参类型有要求,不然会UB
  template <typename... Vs>
  void assignValue(Vs&&... vs) {
    expected_detail::doEmplaceAssign(0, value_, std::forward<Vs>(vs)...);
    which_ = Which::eValue;
  }

  // 同上
  template <typename... Es>
  void assignError(Es&&... es) {
    expected_detail::doEmplaceAssign(0, error_, std::forward<Es>(es)...);
    which_ = Which::eError;
  }

  /*智能选择重新拷贝分配的对象类型*/
  template <typename Other>
  void assign(Other&& rhs) {
    switch (rhs.which_) {
      case Which::eValue:
        this->assignValue(std::forward<Other>(rhs).value());
        break;
      case Which::eError:
        this->assignError(std::forward<Other>(rhs).error());
        break;
      case Which::eEmpty:
      default:
        this->clear();
        break;
    }
  }

  // 以下很简单,判断是否为左值或右值或常量，从而调用性能最佳的函数
  Value& value() & { return value_; }
  const Value& value() const& { return value_; }
  Value&& value() && { return std::move(value_); }
  const Value&& value() const&& {
    return std::move(value_);
  }  // 没什么用的，只是为了类型完整性
  Error& error() & { return error_; }
  const Error& error() const& { return error_; }
  Error&& error() && { return std::move(error_); }
  const Error&& error() const&& {
    return std::move(error_);
  }  // 没什么用的，只是为了类型完整性
};   // struct ExpectedStorage<Value, Error, StorageType::ePODUnion>

/*struct ExpectedStorage调用示意图*/
// Constructor:
// 三种构造函数:eError、eValue、eEmpty三种(使用三种tag进行重载分发选择),默认使用eError
// assign: 三种重新拷贝/移动分配器针对{eError}||{eValue}||{完整对象}
// {eError}||{eValue}将调用doEmplaceAssign函数进行重新分配;

template <typename Value, typename Error>
struct ExpectedUnion {
  union {
    Value value_;
    Error error_;
    char ch_ = 0;
  };
  Which which_ = Which::eEmpty;

  explicit constexpr ExpectedUnion(EmptyTag) noexcept {}
  template <typename... Vs>
  explicit constexpr ExpectedUnion(ValueTag, Vs&&... vs) noexcept(
      noexcept(Value(std::forward<Vs>(vs)...)))
      : value_(std::forward<Vs>(vs)...), which_(Which::eValue) {}
  template <typename... Es>
  explicit constexpr ExpectedUnion(ErrorTag, Es&&... es) noexcept(
      Error(std::forward<Es>(es)...))
      : error_(std::forward<Es>(es)...), which_(Which::eError) {}

  /*不理解为什么拷贝构造和拷贝复制没有对rhs对象进行拷贝*/
  ExpectedUnion(const ExpectedUnion&) {}
  ExpectedUnion(ExpectedUnion&&) noexcept {}
  ExpectedUnion& operator=(const ExpectedUnion&) { return *this; }
  ExpectedUnion& operator=(ExpectedUnion&&) noexcept { return *this; }
  ~ExpectedUnion() {}

  Value& value() & { return value_; }
  const Value& value() const& { return value_; }
  Value&& value() && { return std::move(value_); }
  const Value&& value() const&& { return std::move(value_); }
  Error& error() & { return error_; }
  const Error& error() const& { return error_; }
  Error&& error() && { return std::move(error_); }
  const Error&& error() const&& { return std::move(error_); }
};  /// struct ExpectedUnion

template <typename T>
using IsCopyable =
    std::conjunction<std::is_copy_constructible<T>, std::is_copy_assignable<T>>;

template <typename T>
inline constexpr bool IsCopyable_v = IsCopyable<T>::value;

template <typename T>
using IsMovable =
    std::conjunction<std::is_move_constructible<T>, std::is_move_assignable<T>>;

template <typename T>
inline constexpr bool IsMoveable_v = IsMovable<T>::value;

template <typename T>
using IsNothrowCopyable =
    std::conjunction<std::is_nothrow_copy_constructible<T>,
                     std::is_nothrow_copy_assignable<T>>;

template <typename T>
inline constexpr bool IsNothrowCopyable_v = IsNothrowCopyable<T>::value;

template <typename T>
using IsNothrowMovable = std::conjunction<std::is_nothrow_move_constructible<T>,
                                          std::is_nothrow_move_assignable<T>>;

template <typename T>
inline constexpr bool IsNothrowMoveable_v = IsNothrowMovable<T>::value;

template <typename From, typename To>
using IsConvertible = std::conjunction<std::is_constructible<To, From>,
                                       std::is_assignable<To&, From&>>;

template <typename From, typename To>
inline constexpr bool IsConvertible_v = IsConvertible<To, From>::value;

/*
 * 以下函数用于判断是否能执行相应的操作
 * 使用继承的方式去逐个控制子对象的默认生成的操作
 */
template <typename Derived, bool, bool Noexcept>
struct CopyConstructible {
  constexpr CopyConstructible() = default;
  CopyConstructible(const CopyConstructible& rhs) noexcept(Noexcept) {
    static_cast<Derived*>(this)->assign(static_cast<const Derived&>(rhs));
  }
  constexpr CopyConstructible(CopyConstructible&&) = default;
  CopyConstructible& operator=(const CopyConstructible&) = default;
  CopyConstructible& operator=(CopyConstructible&&) = default;
};  /// struct CopyConstructible

template <typename Derived, bool Noexcept>
struct CopyConstructible<Derived, false, Noexcept> {
  constexpr CopyConstructible() = default;
  CopyConstructible(const CopyConstructible&) = delete;
  constexpr CopyConstructible(CopyConstructible&&) = default;
  CopyConstructible& operator=(const CopyConstructible&) = default;
  CopyConstructible& operator=(CopyConstructible&&) = default;
};  /// struct CopyConstructible<Derived, false, Noexcept>

template <typename Derived, bool, bool Noexcept>
struct MoveConstructible {
  constexpr MoveConstructible() = default;
  constexpr MoveConstructible(const MoveConstructible&) = default;
  MoveConstructible(MoveConstructible&& rhs) noexcept(Noexcept) {
    static_cast<Derived*>(this)->assign(std::move(static_cast<Derived&>(rhs)));
  }
  MoveConstructible& operator=(const MoveConstructible&) = default;
  MoveConstructible& operator=(MoveConstructible&&) = default;
};  /// struct MoveConstructible

template <typename Derived, bool Noexcept>
struct MoveConstructible<Derived, false, Noexcept> {
  constexpr MoveConstructible() = default;
  constexpr MoveConstructible(const MoveConstructible&) = default;
  MoveConstructible(MoveConstructible&&) = delete;
  MoveConstructible& operator=(const MoveConstructible&) = default;
  MoveConstructible& operator=(MoveConstructible&&) = default;
};  /// struct MoveConstructible<Derived, false, Noexcept>

template <typename Derived, bool, bool Noexcept>
struct CopyAssignable {
  constexpr CopyAssignable() = default;
  constexpr CopyAssignable(const CopyAssignable&) = default;
  constexpr CopyAssignable(CopyAssignable&&) = default;
  CopyAssignable& operator=(const CopyAssignable& rhs) noexcept(Noexcept) {
    static_cast<Derived*>(this)->assign(static_cast<const Derived&>(rhs));
    return *this;
  }
  CopyAssignable& operator=(CopyAssignable&&) = default;
};  /// struct CopyAssignable

template <typename Derived, bool Noexcept>
struct CopyAssignable<Derived, false, Noexcept> {
  constexpr CopyAssignable() = default;
  constexpr CopyAssignable(const CopyAssignable&) = default;
  constexpr CopyAssignable(CopyAssignable&&) = default;
  CopyAssignable& operator=(const CopyAssignable&) = delete;
  CopyAssignable& operator=(CopyAssignable&&) = default;
};  /// struct CopyAssignable<Derived, false, Noexcept>

template <typename Derived, bool, bool Noexcept>
struct MoveAssignable {
  constexpr MoveAssignable() = default;
  constexpr MoveAssignable(const MoveAssignable&) = default;
  constexpr MoveAssignable(MoveAssignable&&) = default;
  MoveAssignable& operator=(const MoveAssignable&) = default;
  MoveAssignable& operator=(MoveAssignable&& rhs) noexcept(Noexcept) {
    static_cast<Derived*>(this)->assign(std::move(static_cast<Derived&>(rhs)));
    return *this;
  }
};  /// struct MoveAssignable

template <typename Derived, bool Noexcept>
struct MoveAssignable<Derived, false, Noexcept> {
  constexpr MoveAssignable() = default;
  constexpr MoveAssignable(const MoveAssignable&) = default;
  constexpr MoveAssignable(MoveAssignable&&) = default;
  MoveAssignable& operator=(const MoveAssignable&) = default;
  MoveAssignable& operator=(MoveAssignable&& rhs) = delete;
};  /// struct MoveAssignable<Derived, false, Noexcept>

template <typename Value, typename Error>
struct ExpectedStorage<Value, Error, StorageType::eUnion>  /// eUnion
    : ExpectedUnion<Value, Error>,
      CopyConstructible<
          ExpectedStorage<Value, Error, StorageType::eUnion>,
          StrategyConjunction_v<std::is_copy_constructible, Value, Error>,
          StrategyConjunction_v<std::is_nothrow_copy_constructible, Value,
                                Error>>,
      MoveConstructible<
          ExpectedStorage<Value, Error, StorageType::eUnion>,
          StrategyConjunction_v<std::is_move_constructible, Value, Error>,
          StrategyConjunction_v<std::is_nothrow_move_constructible, Value,
                                Error>>,
      CopyAssignable<ExpectedStorage<Value, Error, StorageType::eUnion>,
                     StrategyConjunction_v<IsCopyable, Value, Error>,
                     StrategyConjunction_v<IsNothrowCopyable, Value, Error>>,
      MoveAssignable<ExpectedStorage<Value, Error, StorageType::eUnion>,
                     StrategyConjunction_v<IsMovable, Value, Error>,
                     StrategyConjunction_v<IsNothrowMovable, Value, Error>> {
  using value_type = Value;
  using error_type = Error;
  using Base = ExpectedUnion<Value, Error>;

  template <typename E = Error, typename = decltype(E{})>
  constexpr ExpectedStorage() noexcept(noexcept(E{})) : Base{ErrorTag{}} {}
  ExpectedStorage(const ExpectedStorage&) = default;
  ExpectedStorage(ExpectedStorage&&) = default;
  ExpectedStorage& operator=(const ExpectedStorage&) = default;
  ExpectedStorage& operator=(ExpectedStorage&&) = default;
  using ExpectedUnion<Value, Error>::ExpectedUnion;
  ~ExpectedStorage() { clear(); }
  void clear() noexcept {
    switch (this->which_) {
      case Which::eValue:
        this->value().~Value();
        break;
      case Which::eError:
        this->error().~Error();
        break;
      case Which::eEmpty:
        break;
    }
    this->which_ = Which::eEmpty;
  }
  bool uninitializedByException() const noexcept {
    return this->which_ == Which::eEmpty;
  }
  template <typename... Vs>
  void assignValue(Vs&&... vs) {
    auto& val = this->value();
    if (this->which_ == Which::eValue) {
      expected_detail::doEmplaceAssign(0, val, std::forward<Vs>(vs)...);
    } else {
      this->clear();
      auto addr =
          const_cast<void*>(static_cast<const void*>(std::addressof(val)));
      ::new (addr) Value(std::forward<Vs>(vs)...);
      this->which_ = Which::eValue;
    }
  }
  template <typename... Es>
  void assignError(Es&&... es) {
    if (this->which_ == Which::eError) {
      expected_detail::doEmplaceAssign(0, this->error(),
                                       std::forward<Es>(es)...);
    } else {
      this->clear();
      ::new ((void*)std::addressof(this->error()))
          Error(std::forward<Es>(es)...);
      this->which_ = Which::eError;
    }
  }
  bool isSelfAssign(const ExpectedStorage* rhs) const { return this == rhs; }
  constexpr bool isSelfAssign(const void*) const { return false; }
  template <typename Other>
  void assign(Other&& rhs) {
    if (isSelfAssign(&rhs)) {
      return;
    }
    switch (rhs.which_) {
      case Which::eValue:
        this->assignValue(std::forward<Other>(rhs).value());
        break;
      case Which::eError:
        this->assignError(std::forward<Other>(rhs).error());
        break;
      case Which::eEmpty:
      default:
        this->clear();
        break;
    }
  }

};  /// struct ExpectedStorage<Value, Error, StorageType::eUnion>

// For small (pointer-sized) trivial types, a struct is faster than a union.
template <typename Value, typename Error>
struct ExpectedStorage<Value, Error, StorageType::ePODStruct> {  /// ePODSttruct
  using value_type = Value;
  using error_type = Error;
  Which which_;
  Error error_;
  Value value_;

  constexpr ExpectedStorage() noexcept
      : which_(Which::eError), error_{}, value_{} {}
  explicit constexpr ExpectedStorage(EmptyTag) noexcept
      : which_(Which::eEmpty), error_{}, value_{} {}
  template <typename... Vs>
  explicit constexpr ExpectedStorage(ValueTag, Vs&&... vs) noexcept(
      noexcept(Value(std::forward<Vs>(vs)...)))
      : which_(Which::eValue), error_{}, value_(std::forward<Vs>(vs)...) {}
  template <typename... Es>
  explicit constexpr ExpectedStorage(ErrorTag, Es&&... es) noexcept(
      noexcept(Error(std::forward<Es>(es)...)))
      : which_(Which::eError), error_(std::forward<Es>(es)...), value_{} {}
  void clear() noexcept {}
  constexpr static bool uninitializedByException() noexcept { return false; }
  template <typename... Vs>
  void assignValue(Vs&&... vs) {
    expected_detail::doEmplaceAssign(0, value_, std::forward<Vs>(vs)...);
    which_ = Which::eValue;
  }
  template <typename... Es>
  void assignError(Es&&... es) {
    expected_detail::doEmplaceAssign(0, error_, std::forward<Es>(es)...);
    which_ = Which::eError;
  }
  template <typename Other>
  void assign(Other&& rhs) {
    switch (rhs.which_) {
      case Which::eValue:
        this->assignValue(std::forward<Other>(rhs).value());
        break;
      case Which::eError:
        this->assignError(std::forward<Other>(rhs).error());
        break;
      case Which::eEmpty:
      default:
        this->clear();
        break;
    }
  }
  Value& value() & { return value_; }
  const Value& value() const& { return value_; }
  Value&& value() && { return std::move(value_); }
  const Value&& value() const&& { return std::move(value_); }
  Error& error() & { return error_; }
  const Error& error() const& { return error_; }
  Error&& error() && { return std::move(error_); }
  const Error&& error() const&& { return std::move(error_); }
};  /// struct ExpectedStorage<Value, Error, StorageType::ePODStruct>

namespace expected_detail_ExpectedHelper {

}  // namespace expected_detail_ExpectedHelper

}  // namespace expected_detail

/******************由此开始都是class Expected的定义内容了*******************/

/*
 * Forward declarations
 */
template <typename Value, typename Error>
class expected;

// 此处与Folly有区别,主要是模板形参顺序
template <typename Value, typename Error>
[[nodiscard]] constexpr expected<std::decay_t<Error>, Value> makeExpected(
    Error&&);

/*
 * Alias for an expected type's associated value_type
 */
template <typename expected>
using ExpectedValueType =
    typename std::remove_reference_t<expected>::value_type;

/**
 * Alias for an expected type's associated error_type
 */
template <typename expected>
using ExpectedErrorType =
    typename std::remove_reference_t<expected>::error_type;

template <typename Value, typename Error>
class expected final : expected_detail::ExpectedStorage<Value, Error> {
  template <typename, typename>
  friend class expected;
  template <typename, typename, expected_detail::StorageType>
  friend struct expected_detail::ExpectedStorage;
  friend struct expected_detail::ExpectedHelper;
  using Base = expected_detail::ExpectedStorage<Value, Error>;
  Base& base() & { return *this; }
  const Base& base() const& { return *this; }
  Base&& base() && { return std::move(*this); }

  struct MakeBadExpectedAccess {
    template <typename E>
    auto operator()(E&& e) {
      return bad_expected_access<Error>(std::forward<E>(e));
    }
  };  // struct MakeBadExpectedAccess

 public:
  using value_type = Value;
  using error_type = Error;

  template <typename U>
  using rebind = expected<U, Error>;

  using promise_type = typename expected_detail::Promise<Value, Error>;

  static_assert(
      !std::is_reference_v<Value>,
      "The Value template argument of expected cannot be a reference type!");

  static_assert(!std::is_abstract_v<Value>,
                "The Value template argument of expected cannot be an abstract "
                "class type!");

  /*
   * Constructors
   */
  template <class B = Base, class = decltype(B{})>
  expected() noexcept(noexcept(B{})) : Base{} {}
  expected(const expected& rhs) = default;
  expected(expected&& rhs) = default;

  template <typename V, typename E,
            HY_REQUIRES_TRAILING(!std::is_same_v<expected<V, E>, expected> &&
                                 std::is_constructible_v<Value, V&&> &&
                                 std::is_constructible_v<Error, E&&>)>
  expected(expected<V, E> rhs) : Base{expected_detail::EmptyTag{}} {
    this->assign(std::move(rhs));
  }

  template <class V, class E,
            HY_REQUIRES_TRAILING(
                !std::is_same<expected<V, E>, expected>::value &&
                !std::is_constructible<Value, V&&>::value &&
                std::is_constructible<expected<Value, Error>, V&&>::value &&
                std::is_constructible<Error, E&&>::value)>
  /* implicit */ expected(expected<V, E> rhs)
      : Base{expected_detail::EmptyTag{}} {
    this->assign(std::move(rhs).then([](V&& v) -> expected<Value, Error> {
      return expected<Value, Error>{v};
    }));
  }

};  // class expected

/*NonMember functions*/
template <typename Value, typename Error>
constexpr expected<std::decay<Value>, Error> makeExpected(Value&&) { /*TODO*/
}

}  // namespace hy

#endif  // HY_EXCEPTED_FOLLY_H_