#ifndef HY_IO_URING_BASE_H_
#define HY_IO_URING_BASE_H_
/*linux*/
#if defined (__linux) && __has_include(<liburing.h>)
#include <liburing.h>
struct io_uring_sqe;
struct io_uring_cqe;
#endif  //__linux

namespace hy {

class IoUringBackend;

struct IoSqeBase 




}   //namespace hy







#endif  //HY_IO_URING_BASE_H_