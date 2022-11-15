#ifndef _INCLUDE__GEARE__CORE__COMPONENTS_
#define _INCLUDE__GEARE__CORE__COMPONENTS_

#include <glm.hpp>
#include <iostream>

namespace geare::core {

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

struct Transform final {
  // TODO: maybe store this as a quaternion? For now this is euler angles
  glm::vec3 rotation;
  glm::vec3 scale;

  explicit Transform(glm::vec3 rotation = glm::vec3(0, 0, 0),
                     glm::vec3 scale = glm::vec3(1, 1, 1))
      : rotation(rotation), scale(scale) {}
};

std::ostream &operator<<(std::ostream &out, const Transform &transform) {
  out << "[Transform rot: {yaw: " << transform.rotation.z
      << ", pitch: " << transform.rotation.y
      << ", roll: " << transform.rotation.z << "}"
      << ", scale: {x: " << transform.scale.x << ", y: " << transform.scale.y
      << ", z: " << transform.scale.z << "}]";
  return out;
}

} // namespace geare::core

#endif
