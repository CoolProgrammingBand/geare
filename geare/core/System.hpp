#ifndef _INCLUDE__GEARE__CORE__SYSTEM_
#define _INCLUDE__GEARE__CORE__SYSTEM_

#include "../utils/Singleton.hpp"
#include "System.hpp"
#include "entt.hpp"

namespace geare::core {

enum struct ComponentAccessType { Const, Mut };

struct SystemContract {
  int global_priority = 0;
  bool call_only_on_main = false;

  using accessed_component_t = std::tuple<entt::id_type, ComponentAccessType>;

  accessed_component_t *component_ids = nullptr;
  std::size_t captured_component_count = 0;

  ~SystemContract() {
    if constexpr (!std::is_trivially_destructible<entt::id_type>::value)
      for (std::size_t i = 0; i < captured_component_count; i++)
        delete &std::get<0>(component_ids[i]);
  }
};

struct System {
  SystemContract contract;

  virtual auto create_component_view(entt::registry &) -> std::byte * {
    return nullptr;
  }

  virtual void tick(std::byte *ptr) {}
  virtual void tick() { return tick(nullptr); }

protected:
  System() {}
  ~System() { contract.~SystemContract(); }
};

template <typename... Ts> struct StaticSystem;

template <typename T> struct StaticSystem<T> : System {
  StaticSystem() {
    auto usage_kind = std::is_const_v<T> ? ComponentAccessType::Const
                                         : ComponentAccessType::Mut;

    // TODO: refactor me?
    this->contract.component_ids = new SystemContract::accessed_component_t[]{
        {entt::type_id<T>().hash(), usage_kind}};
    this->contract.captured_component_count = 1;
  }

  using view_t = decltype(std::declval<entt::registry>().view<T>());

  virtual auto create_component_view(entt::registry &registry)
      -> std::byte * override {
    return (std::byte *)new view_t(registry.view<T>());
  }

  virtual void tick(std::byte *payload) final {
    return tick((view_t *)payload);
  }

  virtual void tick(view_t *group) {}
};

struct FunctionSystem : System {
  FunctionSystem(std::function<void()> func)
      : func([func](std::byte *) { return func(); }) {
    this->contract = SystemContract();
  }
  FunctionSystem(std::function<void(std::byte *)> func) : func(func) {}

  explicit FunctionSystem(SystemContract contract, std::function<void()> func)
      : func([func](std::byte *_) { return func(); }) {
    this->contract = contract;
  }

  virtual void tick(std::byte *payload) override { return func(payload); }

protected:
  std::function<void(std::byte *)> func;
};

} // namespace geare::core

#endif
