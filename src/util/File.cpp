#include "File.h"
#include <asm-generic/errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <cerrno>
#include <cstdio>
#include <string_view>
#include <system_error>
#include "Utility.h"

namespace hy {
    File::File(int fd, bool owns_fd)noexcept : fd_(fd),owns_fd_(owns_fd){

        check(fd_ != -1 || !owns_fd_, );
    }

    File::File(const char* name, int flags, mode_t mode) :
        fd_(::open(name, flags, mode)), owns_fd_(false) {
            if(fd_ == -1) {
                throw std::system_error(format());
            }
            owns_fd_ = true;
        }

    File::File(std::string_view name, int flags, mode_t mode) :
    File(name.data(), flags, mode) {}

    File::File(File&& rhs) noexcept : fd_{rhs.fd_}, owns_fd_{rhs.owns_fd_} {
        rhs.release();
    }

    File& File::operator=(File &&rhs) {
        closeNoThrow();
        swap(rhs);
        return *this;
    }

    File::~File() {
        auto fd = fd_;
        if(!closeNoThrow()) {
            /*检查*/
        }
    }


File File::temporary() {
    FILE* tmpFile = tmpfile();
    check(!tmpFile, []{throw std::system_error{errno, };});
    /*TODO*/
}

int File::release() noexcept {
    int release = fd_;
    fd_ = -1;
    owns_fd_ = false;
    return release;
}

void File::swap(File& other) noexcept {
  using std::swap;
  swap(fd_, other.fd_);
  swap(owns_fd_, other.owns_fd_);
}

void swap(File& a, File& b) noexcept {
  a.swap(b);
}






















    bool File::doTryLock(int op) {
        int r = flockfile(fd_, op | LOCK_NB);
        if(r == -1 && errno == EWOULDBLOCK) {
            return false;
        }
        /*TODO*/
        return true;
    }


    void File::unlock() {
        /*TODO*/
    }


    void File::unlockShared() {
        unlock();
    }

}   //namespace hy