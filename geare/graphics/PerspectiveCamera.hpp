#ifndef _INCLUDE__GEARE__GRAPHICS__PERSPECTIVE_CAMERA_
#define _INCLUDE__GEARE__GRAPHICS__PERSPECTIVE_CAMERA_

#include "../base/Transform.hpp"
#include "../core/System.hpp"
#include "./RenderingSystems.hpp"

namespace geare::graphics {

struct PerspectiveCamera {
  float fov = 90.f;
  float far_plane = .1f;
  float near_plane = 100.f;
};

struct PerspectiveCameraSystem
    : core::StaticSystem<const PerspectiveCamera, const base::Transform> {
  PerspectiveCameraSystem() { this->contract.global_priority = -5; }

  virtual void tick(view_t &camera_entities) override {
    int width, height;
    glfwGetFramebufferSize(windowing::Window::instance().window, &width,
                           &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto entity : camera_entities) {
      auto &camera = camera_entities.get<const PerspectiveCamera>(entity);
      auto &transform = camera_entities.get<const base::Transform>(entity);

      glm::mat4 camera_rotation = glm::eulerAngleYXZ(
          transform.rotation.y, transform.rotation.x, transform.rotation.z);

      auto view =
          camera_rotation *
          glm::translate(glm::identity<glm::mat4>(), -transform.position);

      auto projection = glm::perspective(camera.fov, (float)width / height,
                                         camera.near_plane, camera.far_plane);

      for (int i = 0; i < vao__index_count__mat4___size; i++) {
        auto &[vao, index_count, transform] = vao__index_count__mat4[i];
        glLoadMatrixf(&(projection * view * transform)[0][0]);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
      }
    }

    transformed_meshes_arena.clear();
    glFlush();
  }
};

} // namespace geare::graphics

#endif
