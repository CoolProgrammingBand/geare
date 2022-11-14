#ifndef _INCLUDE__GEARE__CORE__SCENE_
#define _INCLUDE__GEARE__CORE__SCENE_

#include <entt.hpp>

namespace geare::core
{
    
struct Scene : entt::basic_registry<entt::entity> {
    explicit Scene() {}
};

} // namespace geare::core


#endif
