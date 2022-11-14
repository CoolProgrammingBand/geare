#ifndef _INCLUDE__GEARE__CORE__SYSTEM_
#define _INCLUDE__GEARE__CORE__SYSTEM_

#include "../utils/Singleton.hpp"
#include "System.hpp"
#include "entt.hpp"

namespace geare::core {

struct SystemContract {
  int global_priority = 0;
  bool call_only_on_main = false;
  entt::id_type *component_ids;
  std::size_t captured_component_count;

  ~SystemContract() {
    if constexpr (!std::is_trivially_destructible<entt::id_type>::value)
      for (std::size_t i = 0; i < captured_component_count; i++)
        delete (component_ids + i);
  }
};

struct System {
  SystemContract contract;

  virtual void tick(void *ptr) {}
  virtual void tick() { return tick(nullptr); }

protected:
  System() {}
  ~System() { contract.~SystemContract(); }
};

struct FunctionSystem : System {
  FunctionSystem(std::function<void()> func)
      : func([func](void *) { return func(); }) {}
  FunctionSystem(std::function<void(void *)> func) : func(func) {}

  explicit FunctionSystem(SystemContract contract, std::function<void()> func)
      : func([func](void *_) { return func(); }) {
    this->contract = contract;
  }

  virtual void tick(void *payload) override { return func(payload); }

protected:
  std::function<void(void *)> func;
};

} // namespace geare::core

#endif
