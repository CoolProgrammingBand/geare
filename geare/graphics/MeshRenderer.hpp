#ifndef _INCLUDE__GEARE__GRAPHICS__MESH_RENDERER_
#define _INCLUDE__GEARE__GRAPHICS__MESH_RENDERER_

#include "./Mesh.hpp"

namespace geare::graphics {

struct MeshRenderer {
  MeshRenderer() {}

  bool is_dirty = true;
  Mesh *mesh = &BoxMesh;
  GLuint vao = 0;
  GLuint vbo = 0;
  GLuint ebo = 0;
};

} // namespace geare::graphics

#endif
