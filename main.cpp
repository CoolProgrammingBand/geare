#include <iostream>

#include "./geare/windowing/Window.hpp"

using namespace geare::windowing;

int main(void) {
  auto& window = Window::instance();

  window.show();

  return 0;
}
