#ifndef _INCLUDE__GEARE__WINDOWING__WINDOW_
#define _INCLUDE__GEARE__WINDOWING__WINDOW_

#include <tuple>

#include <GLFW/glfw3.h>

#include "../utils/Singleton.hpp"

namespace geare::windowing {

struct Window : utils::Singleton<Window> {
  Window() {
    if (!glfwInit())
      std::terminate();

    window = glfwCreateWindow(640, 480, "a GLFW Window", NULL, NULL);
    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
      glClear(GL_COLOR_BUFFER_BIT);
      glfwSwapBuffers(window);
      glfwPollEvents();
    }

    glfwTerminate();
  }

  ~Window() {}

  void show() {}

protected:
  GLFWwindow *window;
};

} // namespace geare::windowing

#endif
