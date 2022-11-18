#ifndef _INCLUDE__GEARE__UTILS__ARENA_
#define _INCLUDE__GEARE__UTILS__ARENA_

#include <any>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <memory_resource>
#include <vector>

namespace geare::utils {

template <typename FallbackAllocator =
              std::pmr::polymorphic_allocator<std::byte>>
struct Arena final {
  Arena() {
    max_capacity = 4096 * 4;
    top = blob = fallback_allocator.allocate(max_capacity * sizeof(std::byte));
  }

  ~Arena() {
    clear();
    fallback_allocator.deallocate(blob, max_capacity * sizeof(std::byte));
  }

  FallbackAllocator fallback_allocator;
  std::byte filler_byte = (std::byte)0xDD;
  std::byte *blob;
  std::byte *top;
  std::size_t max_capacity;

  bool can_allocate(std::size_t size) {
    return ((std::size_t)top + size) <= (std::size_t)(blob + max_capacity);
  }

  void clear() {}

  std::byte *allocate_raw(std::size_t size) {
    if (!can_allocate(size))
      return nullptr;

    std::byte *data = top;
    std::memset(data, (int)filler_byte, size);
    top += size;
    return data;
  }
};

} // namespace geare::utils

#endif
