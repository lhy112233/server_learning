#ifndef FILE_HY_H_
#define FILE_HY_H_

#include <exception>
#include <string_view>
#include <system_error>
#include "Unexpected.hpp"
#ifdef __linux__
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#endif //__linux__

#include "ExceptionWrapper.h"
#include "Expected_Tiny.hpp"

namespace hy {

/*文件类,代表打开一个文件*/
class File {
public:
  constexpr File() noexcept : fd_(-1), owns_fd_(false) {}
  explicit File(int fd, bool ownsFd = false) noexcept;

  explicit File(std::string_view, int flags = O_RDONLY,
                mode_t mode = 0666);
  explicit File(const char *name, int flags = O_RDONLY, mode_t mode = 0666);
  /*
  //缺点:当输入的是const char*时,由于匹配优先级规则问题,
  //指针会转换为int、unsigned int等内置类型,
  //因而std::string_view得不到匹配,产生了不符合预期的结果
  explicit File(std::string_view name, int flags = O_RDONLY,
                mode_t mode = 0666);
  */

  template <typename... Args>
  static expected<File, std::exception_ptr> makeFile(Args &&...args) noexcept {
    try {
      return File(std::forward<Args>(args)...);
    } catch (const std::system_error &) {
      return expected<File, std::exception_ptr>{hy::unexpected<std::exception_ptr>(std::current_exception())};
    }
  }

  File(const File &) = delete;
  File &operator=(const File &) = delete;
  File(File &&other) noexcept;
  File &operator=(File &&other) noexcept;

  ~File() noexcept;

  [[nodiscard("创建的临时文件夹必须接收")]] static File temporary();

  int fd() const noexcept { return fd_; }

  explicit operator bool() const noexcept { return fd_ != -1; }

  File dup() const;

  File dupCloseOnExec() const;

  void close();

  bool closeNoThrow() noexcept;

  int release() noexcept;

  void swap(File &other) noexcept;

  void lock();
  bool tryLock();
  void unlock();

  void lockShared();
  bool tryLockShared();
  void unlockShared();

private:
  void doLock(int op);
  bool doTryLock(int op);

private:
  int fd_;
  bool owns_fd_;

}; // class File

void swap(File &lhs, File &rhs) noexcept;

} // namespace hy

#endif // FILE_HY_H_