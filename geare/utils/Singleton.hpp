#ifndef _INCLUDE__GEARE__UTILS__SINGLETON_
#define _INCLUDE__GEARE__UTILS__SINGLETON_

namespace geare::utils {

template <typename T, bool per_thread_instance = false> struct Singleton {
  static inline T &instance() {
    if constexpr (per_thread_instance) {
      static thread_local T instance;
      return instance;
    } else {
      static T instance;
      return instance;
    }
  }

  Singleton(const Singleton &) = delete;
  Singleton(Singleton &&) = delete;
  void operator=(const Singleton &) = delete;
  void operator=(Singleton &&) = delete;

protected:
  Singleton() {}
  ~Singleton() {}
};

template <typename T>
using PerThreadSingleton = Singleton<T, true>;

} // namespace geare::utils

#endif
