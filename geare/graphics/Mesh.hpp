#ifndef _INCLUDE__GEARE__GRAPHICS__MESH_
#define _INCLUDE__GEARE__GRAPHICS__MESH_

#include <cstdlib>
#include <glfw.hpp>
#include <glm.hpp>

namespace geare::graphics {

struct Mesh {
  glm::vec3 *vertices;
  std::size_t vertex_count;

  unsigned *indices;
  std::size_t index_count;
};

/*
  4 --- 5
 /|    /|
7 --- 6 |
| 0 --| 1
|/    |/
3 --- 2

0 - 000
1 - 100
2 - 101
3 - 001

4 - 010
5 - 110
6 - 111
7 - 011
*/

static Mesh BoxMesh = {
    .vertices =
        new glm::vec3[8]{
            glm::vec3(-.5, -.5, -.5),
            glm::vec3(0.5, -.5, -.5),
            glm::vec3(0.5, -.5, 0.5),
            glm::vec3(-.5, -.5, 0.5),
            glm::vec3(-.5, 0.5, -.5),
            glm::vec3(0.5, 0.5, -.5),
            glm::vec3(0.5, 0.5, 0.5),
            glm::vec3(-.5, 0.5, 0.5),
        },
    .vertex_count = 8,
    .indices =
        new unsigned[36]{
            0, 1, 2, 2, 3, 0,

            4, 5, 6, 6, 7, 4,

            0, 4, 5, 5, 1, 0,

            7, 6, 2, 2, 3, 7,

            4, 0, 3, 3, 7, 4,

            6, 5, 1, 1, 2, 6,
        },
    .index_count = 36,
};

} // namespace geare::graphics

#endif
