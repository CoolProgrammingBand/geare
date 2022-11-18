#ifndef _INCLUDE__GEARE__BASE__SPATIAL_
#define _INCLUDE__GEARE__BASE__SPATIAL_

#include <glm.hpp>
#include <iostream>

namespace geare::base {

struct Spatial final {
  glm::vec3 position;

  explicit Spatial(glm::vec3 position = glm::vec3(0, 0, 0))
      : position(position) {}
};

std::ostream &operator<<(std::ostream &out, const Spatial &spatial) {
  out << "[Spatial pos: {x: " << spatial.position.x
      << ", y: " << spatial.position.y << ", z: " << spatial.position.z << "}]";
  return out;
}

} // namespace geare::base

#endif
