#ifndef _INCLUDE__GEARE__WINDOWING__WINDOW_
#define _INCLUDE__GEARE__WINDOWING__WINDOW_

#include <tuple>

#include <GLFW/glfw3.h>

#include "../utils/Singleton.hpp"

#include "entt.hpp"

namespace geare::windowing {

struct Window : utils::Singleton<Window> {
  entt::delegate<bool()> on_should_close{
      [](const void *_) -> bool { return true; }, this};
  bool is_alive = false;

  Window() {
    if (!glfwInit())
      std::terminate();

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    window = glfwCreateWindow(640, 480, "a GLFW Window", NULL, NULL);
    glfwMakeContextCurrent(window);

    is_alive = true;
  }

  void tick() {
    if (!is_alive)
      return;

    if (glfwWindowShouldClose(window))
      if (on_should_close())
        // allow the window to die (RIP)
        is_alive = false;

    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    glfwPollEvents();
  }

  ~Window() {
    // TODO: maybe don't call this if GLFW didn't initialize?
    glfwTerminate();
  }

  void show() {
    glfwShowWindow(window);
  }

  void hide() {
    glfwHideWindow(window);
  }

protected:
  GLFWwindow *window;
};

} // namespace geare::windowing

#endif
