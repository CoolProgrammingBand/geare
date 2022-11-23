#ifndef _INCLUDE__GEARE__CORE__WORLD_
#define _INCLUDE__GEARE__CORE__WORLD_

#include "./Executor.hpp"
#include "Scene.hpp"

namespace geare::core {

struct World {
  explicit World() : executor(&active_scene) {}

  Scene active_scene;
  Executor executor;
};

} // namespace geare::core

#endif
