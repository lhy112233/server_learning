#ifndef HY_SOCKETFILEDESCRIPTORMAP_H_
#define HY_SOCKETFILEDESCRIPTORMAP_H_
namespace hy {
namespace net {
namespace detail {
    #ifdef __linux
    #include <unistd.h>
    #endif  //__linux


struct SocketFileDescriptorMap {
    #ifdef _WIN32

    #elif defined (__linux)
    static int close(int fd) noexcept { return ::close(fd);}

    static int fdToSocket(int fd) noexcept { return fd;}
    static int socketToFd(int sock) noexcept { return sock;}
    #endif  //_WIN32
};
}
}
}








#endif  //HY_SOCKETFILEDESCRIPTORMAP_H_