#ifndef _INCLUDE__GEARE__WINDOWING__WINDOW_
#define _INCLUDE__GEARE__WINDOWING__WINDOW_

#include <tuple>

#include <GLFW/glfw3.h>

#include "../core/Clock.hpp"
#include "../utils/Singleton.hpp"

#include "entt.hpp"

namespace geare::windowing {

struct Window : utils::Singleton<Window> {
  entt::delegate<bool()> on_should_close{
      [](const void *_) -> bool { return true; }, this};
  bool is_alive = false;
  double cursor_x;
  double cursor_y;

  bool update_clock = true;

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
    update_clock = true;
    last_tick_time = glfwGetTime();
  }

  void tick() {
    if (!is_alive)
      return;

    auto &clock = core::Clock::instance();
    if (update_clock) {
      double current_tick_time = glfwGetTime();
      clock.global_time = current_tick_time;
      double delta_time = current_tick_time - last_tick_time;
      clock.delta_time = delta_time;
      last_tick_time = current_tick_time;
    }

    if (glfwWindowShouldClose(window))
      if (on_should_close())
        // allow the window to die (RIP)
        is_alive = false;

    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    if (update_clock)
      clock.frame_count++;
    glfwPollEvents();
  }

  ~Window() {
    if (window)
      glfwDestroyWindow(window);

    // TODO: maybe don't call this if GLFW didn't initialize?
    glfwTerminate();
  }

  void show() { glfwShowWindow(window); }

  void hide() { glfwHideWindow(window); }

protected:
  GLFWwindow *window;
  double last_tick_time = 0;

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

} // namespace geare::windowing

#endif
