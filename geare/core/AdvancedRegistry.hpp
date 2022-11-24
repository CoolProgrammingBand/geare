#ifndef _INCLUDE__GEARE__CORE__ADVANCED_REGISTRY_
#define _INCLUDE__GEARE__CORE__ADVANCED_REGISTRY_

#include <entt.hpp>

namespace geare::core {

enum struct ComponentAccessType : char {
  Const,
  Mut,
};

using UniqueComponentIdentifier = entt::id_type;

using ComponentAccess =
    std::pair<UniqueComponentIdentifier, ComponentAccessType>;

template <typename... Ts>
static constexpr std::array<ComponentAccess, sizeof...(Ts)>
    multicomponent_access = {std::pair(entt::type_hash<Ts>::value(),
                                       std::is_const_v<Ts>
                                           ? ComponentAccessType::Const
                                           : ComponentAccessType::Mut)...};

template <typename... Ts>
using getter_view = entt::view<entt::get_t<Ts...>, entt::exclude_t<>>;

struct AdvancedRegistry : entt::registry {
  bool can_access_component(ComponentAccess access) {
    auto [id, access_as] = access;
    return !mut_map[id] &&
           ((access_as == ComponentAccessType::Mut) <= !const_map[id]);
  }

  void mark_component_as_accessed(ComponentAccess access) {
    auto [id, access_as] = access;
    if (access_as == ComponentAccessType::Mut)
      mut_map[id] = true;
    // Mutable components also read values as constant
    const_map[id]++;
  }

  void unmark_accessed_component(ComponentAccess access) {
    auto [id, access_as] = access;
    if (access_as == ComponentAccessType::Mut)
      mut_map[id] = false;
    // Mutable values also read values as constants
    const_map[id]--;
  }

  unsigned count_component_usages(UniqueComponentIdentifier id) {
    return const_map[id] + (unsigned)mut_map[id];
  }

protected:
  template <typename... Ts> struct SimpleView : getter_view<Ts...> {
    std::reference_wrapper<AdvancedRegistry> registry;

    SimpleView(AdvancedRegistry &registry)
        : getter_view<Ts...>(registry.view<Ts...>()), registry(registry) {
      for (auto &access : multicomponent_access<Ts...>)
        registry.mark_component_as_accessed(access);
    }

    ~SimpleView() {
      AdvancedRegistry &registry = this->registry;
      for (auto &access : multicomponent_access<Ts...>)
        registry.unmark_accessed_component(access);
    }
  };

  std::map<UniqueComponentIdentifier, bool> mut_map;
  std::map<UniqueComponentIdentifier, unsigned> const_map;

public:
  template <typename... Ts> auto get_components() -> SimpleView<Ts...> {
    return SimpleView<Ts...>(*this);
  }
};

} // namespace geare::core

#endif
