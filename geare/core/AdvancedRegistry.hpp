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

struct AdvancedRegistry : entt::registry {
  bool is_component_available(ComponentAccess access) {
    auto [id, access_as] = access;
    return !mut_map[id] &&
           ((access_as == ComponentAccessType::Mut) <= !const_map[id]);
  }

  void use_component(ComponentAccess access) {
    auto [id, access_as] = access;
    if (access_as == ComponentAccessType::Mut)
      mut_map[id] = true;
    // Mutable components also read values as constant
    const_map[id]++;
  }

  void release_component(ComponentAccess access) {
    auto [id, access_as] = access;
    if (access_as == ComponentAccessType::Mut)
      mut_map[id] = false;
    // Mutable values also read values as constants
    const_map[id]--;
  }

  unsigned count_component_usages(UniqueComponentIdentifier id) {
    return const_map[id] + (unsigned)mut_map[id];
  }

  std::map<UniqueComponentIdentifier, bool> mut_map;
  std::map<UniqueComponentIdentifier, unsigned> const_map;
};

} // namespace geare::core

#endif
