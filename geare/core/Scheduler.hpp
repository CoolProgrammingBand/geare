#ifndef _INCLUDE__GEARE__CORE__SCHEDULER_
#define _INCLUDE__GEARE__CORE__SCHEDULER_

#include "System.hpp"
#include <vector>

namespace geare::core {

struct Scheduler {
  void add_system(System *system) { return systems.push_back(system); }

  virtual void tick(entt::registry &registry) {
    for (auto &system : systems) {
      auto &contract = system->contract;

      if (contract.captured_component_count > 0) {
        auto* view = system->create_component_view(registry);
        system->tick(view);
      } else {
        system->tick();
      }
    }
  }

  ~Scheduler() {
    for (auto &system : systems) {
      delete &system;
    }
  }

protected:
  std::vector<System *> systems;
};

} // namespace geare::core

#endif
