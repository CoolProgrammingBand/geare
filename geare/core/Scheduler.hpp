#ifndef _INCLUDE__GEARE__CORE__SCHEDULER_
#define _INCLUDE__GEARE__CORE__SCHEDULER_

#include "../utils/Singleton.hpp"
#include "System.hpp"
#include <vector>

namespace geare::core {

struct Scheduler : utils::Singleton<Scheduler> {
  void add_system(System* system) {
    return systems.push_back(system);
  }

  virtual void tick() {
    for (auto &system : systems) {
      system->tick();
    }
  }

protected:
  std::vector<System*> systems;
};

} // namespace geare::core

#endif
