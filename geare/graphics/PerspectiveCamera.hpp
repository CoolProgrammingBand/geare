#ifndef _INCLUDE__GEARE__GRAPHICS__PERSPECTIVE_CAMERA_
#define _INCLUDE__GEARE__GRAPHICS__PERSPECTIVE_CAMERA_

#include "../core/System.hpp"
#include "../base/Transform.hpp"
#include "./RenderingSystems.hpp"

namespace geare::graphics {

struct PerspectiveCamera {
  float fov = 90.f;
  float far_plane = .1f;
  float near_plane = 100.f;
};

struct PerspectiveCameraSystem : core::StaticSystem<PerspectiveCamera> {
  PerspectiveCameraSystem() { this->contract.global_priority = -5; }

  virtual void tick(view_t& view) override {
    int width, height;
    glfwGetFramebufferSize(windowing::Window::instance().window, &width,
                           &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    for (auto entity : view) {
      auto camera = view.get<PerspectiveCamera>(entity);

      auto view = glm::lookAt(glm::vec3(0, 0, 0), glm::vec3(0, 0, -6),
                              glm::vec3(0.0f, 1.0f, 0.0f));

      auto projection = glm::perspective(camera.fov, (float)width / height,
                                         camera.near_plane, camera.far_plane);

      for (int i = 0; i < vao__index_count__mat4___size; i++) {
        auto &[vao, index_count, transform] = vao__index_count__mat4[i];
        glLoadMatrixf(&(view * projection * transform)[0][0]);
        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
      }
    }

    transformed_meshes_arena.clear();
    glFlush();
  }
};

} // namespace geare::graphics

#endif
