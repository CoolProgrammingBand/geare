#include <iostream>

#include "./ext/entt.hpp"
#include "./ext/glm.hpp"

struct Spatial {
    glm::vec3 position;
};

int main(void) {
  entt::registry registry;

  auto entity = registry.create();
  registry.emplace<Spatial>(entity, Spatial());
  std::cout << registry.get<Spatial>(entity).position.x << std::endl;
  registry.destroy(entity);

  return 0;
}
