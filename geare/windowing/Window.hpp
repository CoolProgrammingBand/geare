#ifndef _INCLUDE__GEARE__WINDOWING__WINDOW_
#define _INCLUDE__GEARE__WINDOWING__WINDOW_

#include "../core/Clock.hpp"
#include "../utils/Singleton.hpp"
#include "entt.hpp"
#include "glfw.hpp"
#include <tuple>

namespace geare::windowing {

struct Window : utils::Singleton<Window> {
  entt::delegate<void(Window &)> on_should_close{[](const void *, Window &) {},
                                                 this};
  bool is_alive = false;
  double cursor_x;
  double cursor_y;

  using KeyPressCallback = void(int key, int scancode, int action, int mods);
  using CursorMoveCallback = void(double x, double y);

  entt::delegate<KeyPressCallback> on_key_press{
      [](const void *_, int key, int scancode, int action, int mods) {}};

  entt::delegate<CursorMoveCallback> on_cursor_move{
      [](const void *_, double x, double y) {}};

  Window() {
    if (!glfwInit())
      std::terminate();

    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    window = glfwCreateWindow(640, 480, "a GLFW Window", NULL, NULL);
    glfwMakeContextCurrent(window);

    glfwSetWindowUserPointer(window, (void *)this);
    glfwSetKeyCallback(window, _glfw_key_callback);
    glfwSetCursorPosCallback(window, _glfw_cursor_position_callback);

    is_alive = true;
  }

  void tick() {
    if (!is_alive) {
      glfwDestroyWindow(window);
      window = nullptr;
      return;
    }

    if (glfwWindowShouldClose(window))
      on_should_close(*this);

    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    core::Clock::instance().frame_count++;
    glfwPollEvents();
  }

  ~Window() {
    if (window)
      glfwDestroyWindow(window);

    // TODO: maybe don't call this if GLFW didn't initialize?
    glfwTerminate();
  }

  void close() { is_alive = false; }

  void show() { glfwShowWindow(window); }

  void hide() { glfwHideWindow(window); }

protected:
  GLFWwindow *window = nullptr;

  static auto _window_from_glfw(GLFWwindow *window) -> Window & {
    return *(Window *)glfwGetWindowUserPointer(window);
  }

  static void _glfw_key_callback(GLFWwindow *window, int key, int scancode,
                                 int action, int mods) {
    _window_from_glfw(window).on_key_press(key, scancode, action, mods);
  }

  static void _glfw_cursor_position_callback(GLFWwindow *w, double x,
                                             double y) {
    auto &window = _window_from_glfw(w);
    window.cursor_x = x;
    window.cursor_y = y;
    window.on_cursor_move(x, y);
  }
};

struct WindowSystem : core::System {
  virtual void tick() override { Window::instance().tick(); }
};

} // namespace geare::windowing

#endif
