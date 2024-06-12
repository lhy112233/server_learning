#include "File.h"

namespace hy {
    File::File(int fd,bool ownsFd)noexcept : fd_(fd),owns_fd_(ownsFd){
        
    }



}   //namespace hy