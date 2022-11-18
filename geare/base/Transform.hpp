#ifndef _INCLUDE__GEARE__BASE__TRANSFORM_
#define _INCLUDE__GEARE__BASE__TRANSFORM_

#include <glm.hpp>
#include <iostream>

namespace geare::base {

struct Transform final {
  glm::vec3 position;
  // TODO: maybe store this as a quaternion? For now this is euler angles
  glm::vec3 rotation;
  glm::vec3 scale;

  bool is_dirty = true;
  glm::mat4 mat;

  void refresh() {
    glm::mat4 translate = glm::translate(glm::identity<glm::mat4>(), position);
    glm::mat4 rot = glm::eulerAngleYXZ(rotation.y, rotation.x, rotation.z);
    mat = glm::scale(translate * rot, scale);
  }

  explicit Transform(glm::vec3 rotation = glm::vec3(0, 0, 0),
                     glm::vec3 scale = glm::vec3(1, 1, 1))
      : rotation(rotation), scale(scale) {}
};

std::ostream &operator<<(std::ostream &out, const Transform &transform) {
  out << "[Transform pos: {x: " << transform.position.x
      << ", y: " << transform.position.y << ", z: " << transform.position.z
      << "}, "
      << "rot: {yaw: " << transform.rotation.z
      << ", pitch: " << transform.rotation.y
      << ", roll: " << transform.rotation.z << "}"
      << ", scale: {x: " << transform.scale.x << ", y: " << transform.scale.y
      << ", z: " << transform.scale.z << "}]";
  return out;
}

} // namespace geare::base

#endif
