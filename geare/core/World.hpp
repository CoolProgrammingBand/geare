#ifndef _INCLUDE__GEARE__CORE__WORLD_
#define _INCLUDE__GEARE__CORE__WORLD_

#include "./AdvancedRegistry.hpp"
#include "./Executor.hpp"
#include "Scene.hpp"

namespace geare::core {

struct World {
  explicit World() : executor(&registry) {}

  AdvancedRegistry registry;
  Executor executor;
};

} // namespace geare::core

#endif
