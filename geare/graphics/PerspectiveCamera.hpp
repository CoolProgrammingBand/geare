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

  bool is_dirty = true;
  GLuint fbo;

  GLuint color_tex;
  GLuint depth_and_stencil_rbo;
};

struct PerspectiveCameraSystem
    : core::StaticSystem<PerspectiveCamera, const base::Transform> {
  PerspectiveCameraSystem(RenderDataStorage *storage) : storage(storage) {
    this->contract.global_priority = -5;
  }

  RenderDataStorage *storage;

  virtual void tick(view_t &camera_entities) override {
    int width, height;
    glfwGetFramebufferSize(windowing::Window::instance().window, &width,
                           &height);

    for (auto entity : camera_entities) {
      auto &camera = camera_entities.get<PerspectiveCamera>(entity);
      auto &transform = camera_entities.get<const base::Transform>(entity);

      if (camera.is_dirty) {
        // Create the framebuffer
        glGenFramebuffers(1, &camera.fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, camera.fbo);

        // Generate the color texture
        glGenTextures(1, &camera.color_tex);
        glBindTexture(GL_TEXTURE_2D, camera.color_tex);

        // Bind the color texture to the framebuffer
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                               GL_TEXTURE_2D, camera.color_tex, 0);

        // Create and bind the stencil and depth to the framebuffer
        glGenRenderbuffers(1, &camera.depth_and_stencil_rbo);
        glBindRenderbuffer(GL_RENDERBUFFER, camera.depth_and_stencil_rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width,
                              height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                                  GL_RENDERBUFFER,
                                  camera.depth_and_stencil_rbo);

        camera.is_dirty = false;
      }

      glBindFramebuffer(GL_FRAMEBUFFER, camera.fbo);
      glViewport(0, 0, width, height);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

      glm::mat4 camera_rotation = glm::eulerAngleYXZ(
          transform.rotation.y, transform.rotation.x, transform.rotation.z);

      auto view = camera_rotation * glm::translate(glm::identity<glm::mat4>(),
                                                   -transform.position);

      auto projection = glm::perspective(camera.fov, (float)width / height,
                                         camera.near_plane, camera.far_plane);

      for (int i = 0; i < storage->sample_count; i++) {
        auto &render_data = storage->samples_begin[i];
        auto transform = render_data.model_mat;

        glLoadMatrixf(&(projection * view * transform)[0][0]);
        glBindVertexArray(render_data.vao);
        glDrawElements(GL_TRIANGLES, render_data.index_count, GL_UNSIGNED_INT,
                       0);
      }
    }

    storage->arena.clear();
    glFlush();
  }
};

} // namespace geare::graphics

#endif
