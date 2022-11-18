#ifndef _INCLUDE__GEARE__CORE__TRANSFORM_REFRESHER_
#define _INCLUDE__GEARE__CORE__TRANSFORM_REFRESHER_

#include "./Components.hpp"
#include "./System.hpp"
#include <glm.hpp>

namespace geare::core {

struct TransformRefresherSystem : StaticSystem<Transform> {
  TransformRefresherSystem() { this->contract.global_priority = -5; }

  virtual void tick(view_t &view) final {
    for (auto &e : view) {
      auto &transform = view.get<Transform>(e);
      if (transform.is_dirty)
        transform.refresh();
    }
  }
};

} // namespace geare::core

#endif
