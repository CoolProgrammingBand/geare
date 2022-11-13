#ifndef _INCLUDE__GEARE__CORE__SYSTEM_
#define _INCLUDE__GEARE__CORE__SYSTEM_

#include "../utils/Singleton.hpp"
#include "System.hpp"
#include "entt.hpp"

namespace geare::core {

struct SystemContract {
  int global_priority = 0;
  bool call_only_on_main = false;
};

struct System {
  SystemContract contract;

  virtual void tick() {}

protected:
  System() {}
  ~System() {}
};

} // namespace geare::core

#endif
