#include <iostream>
#include <syncstream>

#include "./geare/core.hpp"
#include "./geare/windowing.hpp"

using namespace geare::windowing;
using namespace geare::core;

struct DownMoverSystem : StaticSystem<Spatial, const Transform> {
  virtual void tick(DownMoverSystem::view_t &view) override {
    for (auto &entry : view) {
      auto &spatial = view.get<Spatial>(entry);
      const auto &transform = view.get<const Transform>(entry);
      spatial.position.y -= 1;
    }
  }
};

int main(void) {
  auto world = World();
  auto &scheduler = world.scheduler;

  auto &root_scene = world.active_scene;
  auto some_entity = root_scene.create();
  root_scene.emplace<Spatial>(some_entity, Spatial());
  root_scene.emplace<Transform>(some_entity, Transform());

  auto &window = Window::instance();

  scheduler.add_system(new ClockSystem());
  scheduler.add_system(new WindowBeginSystem());
  scheduler.add_system(new WindowEndSystem());

  Inputs::instance().register_keycode('X');
  scheduler.add_system(new FunctionSystem([]() {
    if (Inputs::instance().is_key_down('X')) {
      std::cout << "X key pressed!" << std::endl;
    } else if (Inputs::instance().is_key_up('X')) {
      std::cout << "X key released!" << std::endl;
    } else if (Inputs::instance().is_key_held('X')) {
      std::cout << "X key held!" << std::endl;
    }
  }));

  scheduler.add_system(new DownMoverSystem());
  scheduler.add_system(new InputSystem());

  window.show();

  window.on_should_close.connect([](const void *_, Window &window) {
    std::cout << "Should close triggered." << std::endl;
    window.close();
  });

  window.on_key_press.connect(
      [](const void *_, int key, int scancode, int action, int mods) {
        _glfw_keypress_adapter(key, scancode, action, mods);
      });

  while (window.is_alive) {
    scheduler.tick(world.active_scene);
  }

  return 0;
}
