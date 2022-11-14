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
        using return_type =
            decltype(registry.storage(contract.component_ids[0]));

        // TODO: form a Group or a View for all the requested components

        // this is very unsafe, wrong and generally crazy
        std::byte buffer[sizeof(return_type)];
        return_type *memory = (return_type *)&buffer[0];
        *memory = registry.storage(contract.component_ids[0]);
        system->tick(&buffer[0]);
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
