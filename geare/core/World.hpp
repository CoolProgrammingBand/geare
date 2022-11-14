#ifndef _INCLUDE__GEARE__CORE__WORLD_
#define _INCLUDE__GEARE__CORE__WORLD_

#include "Scene.hpp"
#include "Scheduler.hpp"

namespace geare::core {

struct World {
  explicit World() {}

  Scene active_scene;
  Scheduler scheduler;
};

} // namespace geare::core

#endif
