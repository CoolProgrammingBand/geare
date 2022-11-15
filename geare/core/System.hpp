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

  std::size_t view_size;
  virtual void create_component_view(entt::registry &, std::byte *) {}

  virtual void tick(std::byte *ptr) {}
  virtual void tick() { return tick(nullptr); }

protected:
  System() {}
  ~System() { contract.~SystemContract(); }
};

// TODO: test this
template <typename... Ts> struct StaticSystem : System {
  StaticSystem() {
    this->contract.captured_component_count = sizeof...(Ts);
    this->contract.component_ids =
        new SystemContract::accessed_component_t[sizeof...(Ts)]{
            {entt::type_id<Ts>().hash(), std::is_const_v<Ts>
                                             ? ComponentAccessType::Const
                                             : ComponentAccessType::Mut}...};
    this->view_size = sizeof(view_t);
  }

  using view_t = decltype(std::declval<entt::registry>().view<Ts...>());

  virtual void create_component_view(entt::registry &registry,
                                     std::byte *out) override {
    *(view_t *)out = view_t(registry.view<Ts...>());
  }

  virtual void tick(std::byte *payload) final {
    return tick(*(view_t *)payload);
  }

  virtual void tick(view_t &_) = 0;
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
