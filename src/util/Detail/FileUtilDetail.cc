#include "detail/FileUtilDetail.h"
#include <string_view>

namespace hy {
namespace detail {
#ifdef __linux
namespace {
std::string getTemporaryFilePathStringWithoutTempDirectory(
    std::string_view file_path) {
  return file_path.data() + std::string{".XXXXXX"};
}

std::string getTemporaryFilePathStringWithTemporaryDirectory(
    std::string_view temporary_directory) {
  return (temporary_directory.back() == '/')
             ? (temporary_directory.data() +
                std::string{"tempForAtomicWrite.XXXXXX"})
             : (temporary_directory.data() +
                std::string{"/tempForAtomicWrite.XXXXXX"});
}
}  // namespace

/*根据参数组合字符串*/
std::string getTemporaryFilePathString(std::string_view file_path,
                                       std::string_view temporary_directory) {
  return temporary_directory.empty()
             ? getTemporaryFilePathStringWithoutTempDirectory(file_path)
             : getTemporaryFilePathStringWithTemporaryDirectory(
                   temporary_directory);
}

#endif  //__linux
}  // namespace detail
}  // namespace hy