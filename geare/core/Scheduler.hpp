#ifndef _INCLUDE__GEARE__CORE__SCHEDULER_
#define _INCLUDE__GEARE__CORE__SCHEDULER_

#include "../utils/Arena.hpp"
#include "./System.hpp"
#include <algorithm>
#include <vector>

namespace geare::core {

struct Scheduler {
  void add_system(System *system) {
    systems.push_back(system);
    std::push_heap(
        systems.begin(), systems.end(), [](const System *a, const System *b) {
          return a->contract.global_priority < b->contract.global_priority;
        });
    max_view_size = std::max(system->view_size, max_view_size);
  }

  virtual void tick(entt::registry &registry) {
    for (auto &system : systems) {
      auto view = tick_arena.allocate_raw(system->view_size);
      auto &contract = system->contract;

      if (contract.captured_component_count > 0) {
        system->create_component_view(registry, view);
        system->tick(view);
      } else {
        system->tick();
      }
    }

    tick_arena.clear();
  }

  ~Scheduler() {
    for (auto system : systems)
      delete system;
  }

protected:
  utils::Arena<> tick_arena;
  std::size_t max_view_size = 0;
  std::vector<System *> systems;
};

} // namespace geare::core

#endif
