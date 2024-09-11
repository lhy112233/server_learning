#ifndef HY_UNLOCKMUTEX_HPP_
#define HY_UNLOCKMUTEX_HPP_
#include <atomic>

// #define LOCK_FENCE(unlock_mutex_)                      \
//   std::atomic_thread_fence(std::memory_order_acquire); \
//   unlock_mutex_.lock();                                \
//   std::atomic_thread_fence(std::memory_order_release);

// #define UNLOCK_FENCE(unlock_mutex_)                    \
//   std::atomic_thread_fence(std::memory_order_acquire); \
//   unlock_mutex_.unlock();                              \
//   std::atomic_thread_fence(std::memory_order_release);

// //flag是个引用才行呀
// #define TRY_LOCK_FENCE(unlock_mutex_, flag)            \
//   std::atomic_thread_fence(std::memory_order_acquire); \
//   flag = unlock_mutex_.lock();                         \
//   std::atomic_thread_fence(std::memory_order_release);

namespace hy {
class UnlockMutex {
 public:
  using native_handle_type = std::atomic_flag;

  //需要添加内存屏障
  UnlockMutex() = default;

  UnlockMutex(const UnlockMutex&) = delete;
  UnlockMutex(UnlockMutex&&) = delete;
  UnlockMutex& operator=(const UnlockMutex&) = delete;
  UnlockMutex& operator=(UnlockMutex&&) = delete;
  ~UnlockMutex() = default;

  void lock() noexcept {
    while (flag_.test_and_set(std::memory_order::acquire)) {
      flag_.wait(true, std::memory_order::relaxed);
    }
  }

  bool try_lock() noexcept { return !flag_.test_and_set(std::memory_order::acquire); }

  void unlock() noexcept { 
    flag_.clear(std::memory_order::release);
    flag_.notify_one();
   }

  native_handle_type& native_handle() noexcept { return flag_; }

 private:
  //flag_ 为 true 时锁住
  std::atomic_flag flag_;
};

}  //namespace hy

#endif  //HY_UNLOCKMUTEX_HPP_