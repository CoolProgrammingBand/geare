#ifndef _INCLUDE__GEARE__GRAPHICS__RENDERING_SYSTEMS_
#define _INCLUDE__GEARE__GRAPHICS__RENDERING_SYSTEMS_

#include "../base/Transform.hpp"
#include "../core/System.hpp"
#include "../windowing/Window.hpp"
#include "./MeshRenderer.hpp"
#include <glfw.hpp>
#include <glm.hpp>

namespace geare::graphics {

// Collects the geometry and updates all the dirty buffers
struct GeometryCollectionSystem : core::StaticSystem<MeshRenderer> {
  GeometryCollectionSystem() { this->contract.global_priority = 4; }

  virtual void tick(view_t &view) override final {
    for (auto &e : view) {
      auto &mesh_renderer = view.get<MeshRenderer>(e);
      if (mesh_renderer.is_dirty) {
        auto &mesh = *mesh_renderer.mesh;

        glBindVertexArray(mesh_renderer.vao);
        glGenBuffers(1, &mesh_renderer.vbo);

        glBindBuffer(GL_ARRAY_BUFFER, mesh_renderer.vbo);
        glBufferData(GL_ARRAY_BUFFER, mesh.vertex_count * sizeof(glm::vec3),
                     mesh.vertices, GL_STATIC_DRAW);

        glGenBuffers(1, &mesh_renderer.ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_renderer.ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                     mesh.index_count * sizeof(mesh.indices[0]), mesh.indices,
                     GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3),
                              (void *)0);
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh_renderer.ebo);

        mesh_renderer.is_dirty = false;
      }
    }
  }
};

struct RendererSystem
    : core::StaticSystem<MeshRenderer, const base::Transform> {
  virtual void tick(view_t &view) override final {
    int width, height;
    glfwGetFramebufferSize(windowing::Window::instance().window, &width,
                           &height);
    glViewport(0, 0, width, height);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto &e : view) {
      auto &mesh_renderer = view.get<MeshRenderer>(e);
      auto &transform = view.get<const base::Transform>(e);

      auto &mesh = *mesh_renderer.mesh;
      auto mesh_pos = glm::vec3(0, 1, -6);

      auto view = glm::lookAt(glm::vec3(0, 0, 0), mesh_pos,
                              glm::vec3(0.0f, 1.0f, 0.0f));

      auto projection =
          glm::perspective(.90f, (float)width / height, 0.1f, 100.f);
      glLoadMatrixf(&(view * projection * transform.mat)[0][0]);

      glBindVertexArray(mesh_renderer.vao);
      glDrawElements(GL_TRIANGLES, mesh.index_count, GL_UNSIGNED_INT, 0);
      glBindVertexArray(0);
    }

    glFlush();
  }
};

} // namespace geare::graphics

#endif
