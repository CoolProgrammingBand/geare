#ifndef _INCLUDE__GEARE__GRAPHICS__RENDERING_SYSTEMS_
#define _INCLUDE__GEARE__GRAPHICS__RENDERING_SYSTEMS_

#include "../base/Transform.hpp"
#include "../core/System.hpp"
#include "../utils/Arena.hpp"
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

static utils::Arena<> transformed_meshes_arena;
using intermediate_data_t = std::tuple<GLuint, std::size_t, glm::mat4>;
static intermediate_data_t *vao__index_count__mat4;
static std::size_t vao__index_count__mat4___size;

struct PerFrameMeshTransformGenerator
    : core::StaticSystem<MeshRenderer, const base::Transform> {
  PerFrameMeshTransformGenerator() { this->contract.global_priority = -2; }

  virtual void tick(view_t &view) override final {
    /// XXX: maybe dangerous estimate?
    vao__index_count__mat4___size = view.size_hint();
    vao__index_count__mat4 =
        (intermediate_data_t *)transformed_meshes_arena.allocate_raw(
            sizeof(intermediate_data_t) * vao__index_count__mat4___size);

    int i = 0;
    for (auto &e : view) {
      auto &mesh_renderer = view.get<MeshRenderer>(e);
      auto &transform = view.get<const base::Transform>(e);
      auto &mesh = *mesh_renderer.mesh;

      vao__index_count__mat4[i] = {mesh_renderer.vao, mesh.index_count,
                                   transform.mat};

      i++;
    }
  }
};

} // namespace geare::graphics

#endif
