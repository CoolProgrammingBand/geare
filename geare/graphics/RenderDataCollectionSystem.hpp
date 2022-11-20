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

// This might be a temporary solution or we might be keeping it idk
// Service locator might help solve this?
// This is a more general problem of system-to-system communication and the
// solution is unclear, perhaps could be achieved by async systems waiting for
// other systems to finish and then sending data
struct RenderDataStorage {
  utils::Arena<> arena;
  RenderData *samples_begin;
  std::size_t sample_count;
};

struct RenderDataCollectionSystem
    : core::StaticSystem<MeshRenderer, const base::Transform> {
  RenderDataCollectionSystem(RenderDataStorage *storage) : storage(storage) {
    this->contract.global_priority = -2;
  }

  RenderDataStorage *storage;

  virtual void tick(view_t &view) override final {
    /// XXX: maybe dangerous estimate?
    storage->sample_count = view.size_hint();
    storage->samples_begin = (RenderData *)storage->arena.allocate_raw(
        sizeof(RenderData) * storage->sample_count);

    int i = 0;
    for (auto &e : view) {
      auto &mesh_renderer = view.get<MeshRenderer>(e);
      auto &transform = view.get<const base::Transform>(e);
      auto &mesh = *mesh_renderer.mesh;

      storage->samples_begin[i++] =
          RenderData(mesh_renderer.vao, mesh.index_count, transform.mat);
    }
  }
};

} // namespace geare::graphics

#endif
