#include <iostream>

#include "./geare/core/Scheduler.hpp"
#include "./geare/windowing/Window.hpp"

using namespace geare::windowing;
using namespace geare::core;

int main(void) {
  auto &window = Window::instance();
  auto &scheduler = Scheduler::instance();

  scheduler.add_system(new ClockSystem());

  window.show();

  bool should_close = false;

  window.on_should_close.connect(
      [](const void *should_close_ptr) {
        std::cout << "Should close triggered." << std::endl;
        *(bool *)should_close_ptr = true;
        return true;
      },
      &should_close);

  window.on_key_press.connect([](const void *_, int key, int scancode,
                                 int action,
                                 int mods) { std::cout << key << std::endl; });

  window.on_cursor_move.connect([](const void *_, double x, double y) {
    std::cout << x << ' ' << y << std::endl;
  });

  while (!should_close) {
    scheduler.tick();
    window.tick();
  }

  return 0;
}
