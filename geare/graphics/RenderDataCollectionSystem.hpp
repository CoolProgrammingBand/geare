#ifndef _INCLUDE__GEARE__GRAPHICS__RENDER_DATA_SYSTEM_
#define _INCLUDE__GEARE__GRAPHICS__RENDER_DATA_SYSTEM_

#include "../base/Transform.hpp"
#include "../core/System.hpp"
#include "../utils/Arena.hpp"
#include "../windowing/Window.hpp"
#include "./MeshRenderer.hpp"
#include <glfw.hpp>
#include <glm.hpp>

namespace geare::graphics {

struct RenderData {
  GLuint vao;
  std::size_t index_count;
  glm::mat4 model_mat;

  RenderData(GLuint vao, std::size_t index_count, glm::mat4 model_mat)
      : vao(vao), index_count(index_count), model_mat(model_mat) {}
};

static utils::Arena<> raw_render_data_arena;
static RenderData *raw_render_data;
static std::size_t raw_render_data_size;

struct RenderDataCollectionSystem
    : core::StaticSystem<MeshRenderer, const base::Transform> {
  RenderDataCollectionSystem() { this->contract.global_priority = -2; }

  virtual void tick(view_t &view) override final {
    /// XXX: maybe dangerous estimate?
    raw_render_data_size = view.size_hint();
    raw_render_data = (RenderData *)raw_render_data_arena.allocate_raw(
        sizeof(RenderData) * raw_render_data_size);

    int i = 0;
    for (auto &e : view) {
      auto &mesh_renderer = view.get<MeshRenderer>(e);
      auto &transform = view.get<const base::Transform>(e);
      auto &mesh = *mesh_renderer.mesh;

      raw_render_data[i++] =
          RenderData(mesh_renderer.vao, mesh.index_count, transform.mat);
    }
  }
};

} // namespace geare::graphics

#endif
