#ifndef HY_ASYNC_BASE_H_
#define HY_ASYNC_BASE_H_

/*linux*/

#include <atomic>
#include <cstddef>
#include <deque>
#include <functional>
#include <mutex>
#include <ostream>
#include <span>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>
#ifdef __linux
#include <bits/types/struct_iovec.h>
#include <sys/types.h>
#endif  //__linux

namespace hy {
/*Forward Decleared*/
class AsyncIOOp;
class IoUringOp;
class AsyncBase;

class AsyncBaseOp {
  friend class AsyncBase;

 public:
  using NotificationCallback = ::std::function<void(AsyncBaseOp*)>;
  enum class State {
    UNINITIALIZED,  ///无初始化
    INITIALIZED,    ///初始化
    PENDING,        ///未决的
    COMPLETED,      ///完成的
    CANCELED,       ///取消
  };

  explicit AsyncBaseOp(const NotificationCallback& cb);
  explicit AsyncBaseOp(NotificationCallback&& cd = NotificationCallback()) noexcept;
  AsyncBaseOp(const AsyncBaseOp&) = delete;
  virtual ~AsyncBaseOp();

  virtual void pread(int fd, void* buf, std::size_t size, ::off_t start) = 0;
  void pread(int fd, std::span<unsigned char*> range, ::off_t start);

  virtual void preadv(int fd, const ::iovec* iov, int iovcnt,
                      ::off_t start) = 0;
  virtual void pread(int fd, void* buf, std::size_t size, ::off_t satrt, int);

  virtual void pwrite(int fd, const void* buf, size_t size, off_t start) = 0;
  void pwrite(int fd, std::span<const unsigned char*> range, off_t start);
  virtual void pwritev(int fd, const iovec* iov, int iovcnt, off_t start) = 0;
  virtual void pwrite(int fd, const void* buf, size_t size, off_t start,
                      int /*buf_index*/) {
    pwrite(fd, buf, size, start);
  }

  virtual AsyncIOOp* getAsyncIOOp() = 0;
  virtual IoUringOp* getIoUringOp() = 0;

  virtual void toString(std::ostream& os) const = 0;

  State state() const noexcept { return state_; }

  void* getUserData() const noexcept { return userData_; }

  void setUserData(void* userData) noexcept { userData_ = userData; }

  virtual void reset(const NotificationCallback& cb) = 0;

  virtual void reset(NotificationCallback&& cb = NotificationCallback{}) = 0;

  template <typename T, std::enable_if_t<std::is_same_v<std::remove_cvref_t<T>,
                                                        NotificationCallback>,
                                         int> = 1>
  void setNotificationCallback(T&& cd) {
    return std::forward<T>(cd);
  }

  NotificationCallback getNotificationCallback() noexcept(
      noexcept(std::is_nothrow_move_constructible_v<decltype(cb_)>)) {
    return std::move(cb_);
  }

  ::ssize_t result() const;

  static std::string fd2name(int fd);

 protected:
  void init();
  void start();
  void unstart();
  void complete(::ssize_t result);
  void cancel();

  NotificationCallback cb_;
  std::atomic<State> state_;
  ::ssize_t result_;
  void* userData_{nullptr};

};  //class AsyncBaseOp

std::ostream& operator<<(std::ostream& os, const AsyncBaseOp& op);
std::ostream& operator<<(std::ostream& os, AsyncBaseOp::State state);

class AsyncBase {
 public:
  using Op = AsyncBaseOp;

  enum class PollMode {
    NOT_POLLABLE,
    POLLABLE,
  };

  /*Constructors*/
  explicit AsyncBase(std::size_t capacity,
                     PollMode poll_mode = PollMode::NOT_POLLABLE);
  AsyncBase(const AsyncBase&) = delete;
  AsyncBase& operator=(const AsyncBase&) = delete;
  virtual ~AsyncBase();

  virtual void initializeContext() = 0;

  std::span<Op**> wait(std::size_t min_requests);

  std::span<Op**> cancel();

  std::size_t pending() const noexcept { return pending_; }

  std::size_t capacity() const noexcept { return capacity_; }

  std::size_t totalSubmit() const noexcept { return submitted_; }

  int pollFd() const noexcept { return pollFd_; }

  std::span<Op**> pollCompleted();

  void submit(Op* op);

  int submit(std::span<Op**> ops);

 protected:
  virtual int draiPollFd() = 0;
  void complete(Op* op, ::ssize_t result) { op->complete(result); }

  void cancel(Op* op) { op->cancel(); }

  bool isInit() const { return init_.load(std::memory_order_relaxed); }

  void decrementPending(std::size_t num = 1);
  virtual int submitOne(AsyncBase::Op* op) = 0;
  virtual int submitRange(std::span<AsyncBase::Op**> ops) = 0;

  enum class WaitType {
    COMPLETE,
    CANCEL,
  };

  virtual std::span<AsyncBase::Op**> doWait(WaitType type,
                                            std::size_t minRequests,
                                            std::size_t maxRequests,
                                            std::vector<Op*>& result) = 0;

  /*Datas*/
  std::atomic<bool> init_{false};
  std::mutex init_mutex_;

  std::atomic<std::size_t> pending_{0};
  std::atomic<size_t> submitted_{0};
  const size_t capacity_;
  const PollMode pollMode_;
  int pollFd_{-1};
  std::vector<Op*> completed_;
  std::vector<Op*> canceled_;

};  //class AsyncBase

class AsyncBaseQueue {
 public:
  using OpFactory = std::function<AsyncBaseOp*()>;

  explicit AsyncBaseQueue(AsyncBase* async_base);
  ~AsyncBaseQueue();

  std::size_t queued() const noexcept { return queue_.size(); }

  void submit(AsyncBaseOp* op);
  void submit(const OpFactory& op);
  void submit(OpFactory&& op);

 private:
  void onCompleted(AsyncIOOp* op);
  void maybeDequeue();

  AsyncBase* async_base_;
  std::deque<OpFactory> queue_;
};  //class AsyncBaseQueue

}  //namespace hy

#endif  //HY_ASYNC_BASE_H_