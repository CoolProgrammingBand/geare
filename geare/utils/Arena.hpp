#ifndef _INCLUDE__GEARE__UTILS__ARENA_
#define _INCLUDE__GEARE__UTILS__ARENA_

#include <any>
#include <concepts>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <memory_resource>
#include <vector>

namespace geare::utils {

template <typename FallbackAllocator =
              std::pmr::polymorphic_allocator<std::byte>>
struct Arena final {
  /// XXX: this allocator does not respect alignment

  Arena() {
    max_capacity = 4096 * 4;
    top = blob = fallback_allocator.allocate(max_capacity * sizeof(std::byte));
  }

  ~Arena() {
    clear();
    fallback_allocator.deallocate(blob, max_capacity * sizeof(std::byte));
  }

  struct _NonTriviallyDestructibleHandle {
    virtual ~_NonTriviallyDestructibleHandle() {}
  };

  bool can_allocate(std::size_t size) {
    return ((std::size_t)top + size) <= (std::size_t)(blob + max_capacity);
  }

  void clear() {
    for (auto &handle : destructor_handles)
      handle->~_NonTriviallyDestructibleHandle();

    destructor_handles.clear();
  }

  template <std::default_initializable T> T *allocate() {
    auto raw = allocate_raw<T>();
    if (!raw)
      return nullptr;
    new (std::launder(raw)) T();

    if constexpr (!std::is_trivially_destructible_v<T>) {
      auto *handle = allocate_raw<_NonTriviallyDestructibleInner<T>>();
      new (handle) _NonTriviallyDestructibleInner<T>(*raw);
      destructor_handles.push_back(handle);
    }

    return raw;
  }

  template <std::copy_constructible T> T *allocate(const T &value) {
    auto *raw = allocate_raw<T>();
    if (!raw)
      return nullptr;
    new (std::launder(raw)) T(value);

    if constexpr (!std::is_trivially_destructible_v<T>) {
      auto *handle = allocate_raw(sizeof(_NonTriviallyDestructibleInner<T>));
      new (handle) _NonTriviallyDestructibleInner<T>(*raw);
      destructor_handles.push_back(handle);
    }

    return raw;
  }

  template <std::move_constructible T> T *allocate(T &&value) {
    auto *raw = allocate_raw<T>();
    if (!raw)
      return nullptr;
    new (std::launder(raw)) T();

    if constexpr (!std::is_trivially_destructible_v<T>) {
      auto *handle = allocate_raw(sizeof(_NonTriviallyDestructibleInner<T>));
      new (handle) _NonTriviallyDestructibleInner<T>(*raw);
      destructor_handles.push_back(handle);
    }

    return raw;
  }

  std::byte *allocate_raw(std::size_t size) {
    if (!can_allocate(size))
      return nullptr;

    std::byte *data = top;
    std::memset(data, (int)filler_byte, size);
    top += size;
    return data;
  }

  template <typename T> T *allocate_raw() {
    return (T *)allocate_raw(sizeof(T));
  }

protected:
  template <typename T>
  struct _NonTriviallyDestructibleInner : _NonTriviallyDestructibleHandle {
    _NonTriviallyDestructibleInner(T &&v) : data(v) {}
    _NonTriviallyDestructibleInner(const T &v) : data(v) {}

    ~_NonTriviallyDestructibleInner() override {}

    T data;
  };

  FallbackAllocator fallback_allocator;
  std::vector<_NonTriviallyDestructibleHandle *> destructor_handles;
  std::byte filler_byte = (std::byte)0xDD;
  std::byte *blob;
  std::byte *top;
  std::size_t max_capacity;
};

} // namespace geare::utils

#endif
