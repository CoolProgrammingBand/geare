#ifndef _INCLUDE__GEARE__UTILS__SINGLETON_
#define _INCLUDE__GEARE__UTILS__SINGLETON_

namespace geare::utils {

template <typename T> struct Singleton {
  static inline T &instance() {
    static T instance;
    return instance;
  }

  Singleton(const Singleton &) = delete;
  Singleton(Singleton &&) = delete;
  void operator=(const Singleton &) = delete;
  void operator=(Singleton &&) = delete;

protected:
  Singleton() {}
  ~Singleton() {}
};

} // namespace geare::utils

#endif
