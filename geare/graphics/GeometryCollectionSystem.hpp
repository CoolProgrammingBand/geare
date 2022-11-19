#ifndef _INCLUDE__GEARE__GRAPHICS__GEOMETRY_COLLECTION_SYSTEM_
#define _INCLUDE__GEARE__GRAPHICS__GEOMETRY_COLLECTION_SYSTEM_

#include "../core/System.hpp"
#include "./MeshRenderer.hpp"

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

} // namespace geare::graphics

#endif
