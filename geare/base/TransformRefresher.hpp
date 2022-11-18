#ifndef _INCLUDE__GEARE__BASE__TRANSFORM_REFRESHER_
#define _INCLUDE__GEARE__BASE__TRANSFORM_REFRESHER_

#include "../core/System.hpp"
#include "./Transform.hpp"
#include <glm.hpp>

namespace geare::base {

struct TransformRefresherSystem : core::StaticSystem<Transform> {
  TransformRefresherSystem() { this->contract.global_priority = -5; }

  virtual void tick(view_t &view) final {
    for (auto &e : view) {
      auto &transform = view.get<Transform>(e);
      if (transform.is_dirty)
        transform.refresh();
    }
  }
};

} // namespace geare::base

#endif
