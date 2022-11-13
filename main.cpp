#include <iostream>

#include "./geare/core.hpp"
#include "./geare/windowing.hpp"

using namespace geare::windowing;
using namespace geare::core;

int main(void) {
  auto &window = Window::instance();
  auto &scheduler = Scheduler::instance();

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
    scheduler.tick();
  }

  return 0;
}
