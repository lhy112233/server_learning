#include "io/async/AsyncBase.h"
#include <asm-generic/errno-base.h>
#include <ostream>

namespace hy {

AsyncBaseOp::AsyncBaseOp(const NotificationCallback& cb) : cb_(cb), state_(State::UNINITIALIZED),
result_(-EINVAL) {}

AsyncBaseOp::AsyncBaseOp(NotificationCallback&& cb) noexcept : cb_(std::move(cb)), state_(State::UNINITIALIZED),
result_(-EINVAL) {}











namespace {

#define X(c) \
  case c:    \
    return #c

const char* asyncIoOpStateToString(AsyncBaseOp::State state) {
  switch (state) {
    X(AsyncBaseOp::State::UNINITIALIZED);
    X(AsyncBaseOp::State::INITIALIZED);
    X(AsyncBaseOp::State::PENDING);
    X(AsyncBaseOp::State::COMPLETED);
    X(AsyncBaseOp::State::CANCELED);
  }
  return "<INVALID AsyncBaseOp::State>";
}
#undef X
} // namespace




std::ostream& operator<<(std::ostream& os, const AsyncBaseOp& op) {
//   op.toStream(os);
  return os;
}


std::ostream operator<<(std::ostream& os, AsyncBaseOp::State state) {
    return os << asyncIoOpStateToString(state);
}


}   //namespace hy