#ifndef _INCLUDE__GEARE__CORE__ADVANCED_REGISTRY_
#define _INCLUDE__GEARE__CORE__ADVANCED_REGISTRY_

#include <span>

#include <entt.hpp>

#include "./Logger.hpp"

namespace geare::core {

enum struct ComponentBorrowKind : char {
  Const,
  Mut,
};

using UniqueComponentIdentifier = entt::id_type;

using ComponentBorrowDescriptor =
    std::pair<UniqueComponentIdentifier, ComponentBorrowKind>;

template <typename T>
constexpr auto make_component_access() -> ComponentBorrowDescriptor {
  return std::pair(entt::type_hash<T>::value(), std::is_const_v<T>
                                                    ? ComponentBorrowKind::Const
                                                    : ComponentBorrowKind::Mut);
}
template <typename... Ts>
static constexpr std::array<ComponentBorrowDescriptor, sizeof...(Ts)>
    multicomponent_access = {make_component_access<Ts>()...};

template <typename... Ts>
using getter_view = entt::view<entt::get_t<Ts...>, entt::exclude_t<>>;

struct AdvancedRegistry : entt::registry {
  bool can_borrow(ComponentBorrowDescriptor borrow) {
    auto [id, kind] = borrow;
    auto [consts, is_mut] = active_borrows[id];
    if (is_mut)
      return false;
    if (kind == ComponentBorrowKind::Const)
      return true;
    else
      return !consts;
  }

  bool can_borrow(std::span<const ComponentBorrowDescriptor> borrows) {
    auto can = true;
    for (std::size_t i = 0; i < borrows.size() && can; i++)
      can &= this->can_borrow(borrows[i]);
    return can;
  }

  void _borrow_component_unsafe(ComponentBorrowDescriptor borrow) {
    auto [id, access_as] = borrow;
    auto &[consts, is_mut] = active_borrows[id];
    if (access_as == ComponentBorrowKind::Mut)
      is_mut = true;
    else
      consts++;
    on_component_borrow(borrow);
  }

  void _release_component_unsafe(ComponentBorrowDescriptor borrow) {
    auto [id, previously_borrowed_as] = borrow;
    auto &[consts, is_mut] = active_borrows[id];
    if (previously_borrowed_as == ComponentBorrowKind::Mut)
      is_mut = false;
    else
      consts--;
    on_component_released(borrow);
  }

  unsigned count_component_usages(UniqueComponentIdentifier id) {
    return active_borrows[id].count();
  }

  template <typename... Ts> struct SimpleView : getter_view<Ts...> {
    std::reference_wrapper<AdvancedRegistry> registry;

    SimpleView(AdvancedRegistry &registry)
        : getter_view<Ts...>(registry.view<Ts...>()), registry(registry) {
      for (auto &borrow : multicomponent_access<Ts...>)
        registry._borrow_component_unsafe(borrow);
    }

    auto get(AdvancedRegistry::entity_type entity) {
      return ((AdvancedRegistry&)registry).get<Ts...>(entity);
    }

    // TODO: remake this with RAII
    void release() {
      for (auto &borrow : multicomponent_access<Ts...>)
        this->registry.get()._release_component_unsafe(borrow);
    }
  };

  struct ActiveBorrow : std::pair<unsigned, bool> {
    using std::pair<unsigned, bool>::pair;

    inline bool &is_mut_borrowed() { return this->second; }
    inline unsigned &const_borrows() { return this->first; }

    inline unsigned count() {
      return const_borrows() + (unsigned)is_mut_borrowed();
    }
  };

  std::map<UniqueComponentIdentifier, ActiveBorrow> active_borrows;

  entt::delegate<void(ComponentBorrowDescriptor)> on_component_released{
      [](const void *, ComponentBorrowDescriptor) {}};

  entt::delegate<void(ComponentBorrowDescriptor)> on_component_borrow{
      [](const void *, ComponentBorrowDescriptor) {}};

  template <typename... Ts> auto get_components() -> SimpleView<Ts...> {
    return SimpleView<Ts...>(*this);
  }
};

} // namespace geare::core

#endif
