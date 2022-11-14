#include <iostream>

#include "./geare/core.hpp"
#include "./geare/windowing.hpp"

using namespace geare::windowing;
using namespace geare::core;

struct DownMoverSystem : StaticSystem<Spatial> {
  virtual void tick(DownMoverSystem::view_t *v) override {
    auto& view = *v;
    for (auto &entry : view) {
      auto& spatial = view.get<Spatial>(entry);
      spatial.position.y -= 1;
      std::cout << "Moved " << (int)entry
                << " down by one, now at y=" << spatial.position.y << std::endl;
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
  scheduler.add_system(new WindowSystem());
  scheduler.add_system(new FunctionSystem([]() {
    if (Inputs::instance().get_key_down('X')) {
      std::cout << "X key pressed!" << std::endl;
    } else if (Inputs::instance().get_key_up('X')) {
      std::cout << "X key released!" << std::endl;
    } else if (Inputs::instance().get_key_held('X')) {
      std::cout << "X key held!" << std::endl;
    }
  }));
  scheduler.add_system(new DownMoverSystem());
  scheduler.add_system(new InputSystem());

  Inputs::instance().capture_keycode('X');

  window.show();

  window.on_should_close.connect([](const void *_, Window &window) {
    std::cout << "Should close triggered." << std::endl;
    window.close();
  });

  window.on_key_press.connect(
      [](const void *_, int key, int scancode, int action, int mods) {
        _glfw_keypress_adapter(key, scancode, action, mods);
      });

  while (true) {
    scheduler.tick(world.active_scene);
  }

  return 0;
}
