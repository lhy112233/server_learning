#ifndef HY_FILE_UTIL_H_
#define HY_EXPECTED_HPP_

#include <bits/types/struct_iovec.h>
#include <cstddef>
#include "net/NetworkSocket.h"

/*linux*/
#ifdef __linux
#include <sys/types.h>
namespace hy {
namespace file_util {

int openNoEintr(const char* name, int flags, mode_t = 0666);

int closeNoEintr(int fd);
int closeNoEintr(net::NetworkSocket fd);
int dupNoEintr(int fd);
int dup2NoEintr(int oldFd, int newFd);
int fsyncNoEintr(int fd);
int fdatasyncNoEintr(int fd);
int ftruncateNoEintr(int fd, ::off_t len);
int truncateNoEintr(const char* path, ::off_t len);
int floatkNoEintr(int fd, int operation);
int shutdownNoEintr(net::NetworkSocket fd, int how);

::ssize_t readNoEintr(int fd, void* buf, std::size_t count);
::ssize_t preadNoInt(int fd, void* buf, std::size_t count, ::off_t offset);
::ssize_t readvNoInt(int fd, const ::iovec* iov, int count);
::ssize_t preadvNoInt(int fd, const ::iovec* iov, int count, ::off_t offset);


::ssize_t writeNoInt(int fd, const void* buf, std::size_t count);
::ssize_t pwriteNoInt(int fd, const void* buf, std::size_t count, ::off_t offset);
::ssize_t writevNoInt(int fd, const ::iovec* iov, int count);
::ssize_t pwritevNoInt(int fd, const ::iovec* iov, int count, ::off_t offset);


}  // namespace file_util
}  // namespace hy

#endif  //__linux
#endif  //HY_FILE_UTIL_H_