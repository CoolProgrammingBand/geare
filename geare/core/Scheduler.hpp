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
    max_view_size = std::max(system->view_size, max_view_size);
  }

  virtual void tick(entt::registry &registry) {
    auto *view = (std::byte *)alloca(max_view_size);
    for (auto &system : systems) {

      auto &contract = system->contract;

      if (contract.captured_component_count > 0) {
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
  std::size_t max_view_size = 0;
  std::vector<System *> systems;
};

} // namespace geare::core

#endif
