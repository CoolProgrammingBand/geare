#include <iostream>

#include "./geare/windowing/Window.hpp"

using namespace geare::windowing;

int main(void) {
  auto& window = Window::instance();

  window.show();

  bool should_close = false;
  window.on_should_close.connect([](const void* should_close_ptr){
    std::cout << "Should close triggered." << std::endl;
    *(bool*)should_close_ptr = true;
    return true;
  }, &should_close);

  while (!should_close) {
    window.tick();
  }

  return 0;
}
