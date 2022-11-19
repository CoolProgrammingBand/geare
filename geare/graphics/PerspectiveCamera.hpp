#ifndef _INCLUDE__GEARE__GRAPHICS__PERSPECTIVE_CAMERA_
#define _INCLUDE__GEARE__GRAPHICS__PERSPECTIVE_CAMERA_

#include "../base/Transform.hpp"
#include "../core/System.hpp"
#include "./RenderDataCollectionSystem.hpp"

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

      auto view = camera_rotation * glm::translate(glm::identity<glm::mat4>(),
                                                   -transform.position);

      auto projection = glm::perspective(camera.fov, (float)width / height,
                                         camera.near_plane, camera.far_plane);

      for (int i = 0; i < raw_render_data_size; i++) {
        auto &render_data = raw_render_data[i];
        auto transform = render_data.model_mat;

        glLoadMatrixf(&(projection * view * transform)[0][0]);
        glBindVertexArray(render_data.vao);
        glDrawElements(GL_TRIANGLES, render_data.index_count, GL_UNSIGNED_INT,
                       0);
      }
    }

    raw_render_data_arena.clear();
    glFlush();
  }
};

} // namespace geare::graphics

#endif
