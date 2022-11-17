#ifndef _INCLUDE__GEARE__CORE__SCHEDULER_
#define _INCLUDE__GEARE__CORE__SCHEDULER_

#include "System.hpp"
#include <algorithm>
#include <vector>

namespace geare::core {

struct Scheduler {
  void add_system(System *system) {
    // TODO: more efficient push
    systems.push_back(system);
    std::push_heap(
        systems.begin(), systems.end(), [](const System *a, const System *b) {
          return a->contract.global_priority < b->contract.global_priority;
        });
  }

  virtual void tick(entt::registry &registry) {
    for (auto &system : systems) {
      auto &contract = system->contract;

      if (contract.captured_component_count > 0) {
        // XXX: dangerous, alloca may stackoverflow
        // It shouldn't, but it may. Fix that.
        auto *view = (std::byte *)alloca(system->view_size);
        system->create_component_view(registry, view);
        system->tick(view);
      } else {
        system->tick();
      }
    }
  }

  ~Scheduler() {
    for (auto system : systems)
      delete system;
  }

protected:
  std::vector<System *> systems;
};

} // namespace geare::core

#endif
