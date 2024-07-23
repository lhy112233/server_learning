#include "Exception.h"
#include <atomic>
#include <cassert>
#include <cstddef>
#include <cstring>

namespace hy {

struct exception_shared_string::state {
  // refcount ops use relaxed order since the string is immutable: side-effects
  // need not be made visible to the destructor since there are none
  static constexpr auto relaxed = std::memory_order_relaxed;
  std::atomic<std::size_t> refs{0u};
  const std::size_t size{0u};
  static constexpr std::size_t object_size(std::size_t const len) noexcept {
    return sizeof(state) + len + 1u;
  }
  static state* make(const char* const str, const std::size_t len) {
    constexpr auto align = std::align_val_t{alignof(state)};
    assert(len == std::strlen(str));
    auto addr = ::operator new(object_size(len), align);
    return new (addr) state(str, len);
  }
  state(char const* const str, std::size_t const len) noexcept : size{len} {
    std::memcpy(static_cast<void*>(this + 1u), str, len + 1u);
  }
  char const* what() const noexcept {
    return static_cast<const char*>(static_cast<const void*>(this + 1u));
  }
  void copy() noexcept { refs.fetch_add(1u, relaxed); }
  void ruin() noexcept {
    constexpr auto align = std::align_val_t{alignof(state)};
    if (!refs.load(relaxed) || !refs.fetch_sub(1u, relaxed)) {
      ::operator delete(this, object_size(size), align);
    }
  }
};

exception_shared_string::exception_shared_string(const char* const str)
    : hy::exception_shared_string{str, std::strlen(str)} {}
exception_shared_string::exception_shared_string(const char* const str,
                                                 const std::size_t len)
    : state_{state::make(str, len)} {}
exception_shared_string::exception_shared_string(const exception_shared_string& rhs) noexcept
: state_{(rhs.state_->copy(), rhs.state_)} {} 
exception_shared_string::~exception_shared_string() {
  state_->ruin();
}

const char* exception_shared_string::what() const noexcept {
  return state_->what();
}

}  //namespace hy
