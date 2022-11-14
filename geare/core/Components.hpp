#ifndef _INCLUDE__GEARE__CORE__COMPONENTS_
#define _INCLUDE__GEARE__CORE__COMPONENTS_

#include <glm.hpp>

namespace geare::core {

struct Spatial final {
  glm::vec3 position;

  Spatial(glm::vec3 position = glm::vec3(0, 0, 0)) : position(position) {}
};

struct Transform final {
  // TODO: maybe store this as a quaternion? For now this is euler angles
  glm::vec3 rotation;
  glm::vec3 scale;

  explicit Transform(glm::vec3 rotation = glm::vec3(0, 0, 0),
                     glm::vec3 scale = glm::vec3(1, 1, 1))
      : rotation(rotation), scale(scale) {}
};

} // namespace geare::core

#endif
