#include <iostream>

#include "./geare/core/Scheduler.hpp"
#include "./geare/windowing/Window.hpp"

using namespace geare::windowing;
using namespace geare::core;

int main(void) {
  auto &window = Window::instance();
  auto &scheduler = Scheduler::instance();

  scheduler.add_system(new ClockSystem());
  scheduler.add_system(new WindowSystem());

  window.show();

  window.on_should_close.connect([](const void *_, Window &window) {
    std::cout << "Should close triggered." << std::endl;
    window.close();
  });

  window.on_key_press.connect([](const void *_, int key, int scancode,
                                 int action,
                                 int mods) { std::cout << key << std::endl; });

  window.on_cursor_move.connect([](const void *_, double x, double y) {
    std::cout << x << ' ' << y << std::endl;
  });

  while (true) {
    scheduler.tick();
  }

  return 0;
}
