#include "IPAddress_detail.h"
#include <sys/socket.h>
#include <cstddef>
#include <stdexcept>
#include "Preprocessor.h"
#if (__cplusplus < CPP20)
#include <fmt/core.h>
using fmt::format;
#else
#include <format>
using std::format;
#endif

namespace hy {
namespace detail {
const char* familyNameStrDefault(sa_family_t family) {
  return format(sa_family_t({}), family);
}

[[noreturn]] void getNthMSBitImplThrow(std::size_t bitCount,
                                       sa_family_t family) {
  throw std::invalid_argument(
      format("Bit index must be < {} for addresses of type: {}"), bitCount,
      familyNameStr(family));
}

}  // namespace detail
}  // namespace hy