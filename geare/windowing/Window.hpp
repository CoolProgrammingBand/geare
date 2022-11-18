#ifndef _INCLUDE__GEARE__WINDOWING__WINDOW_
#define _INCLUDE__GEARE__WINDOWING__WINDOW_

#include "../base/Clock.hpp"
#include "../graphics/Mesh.hpp"
#include "../utils/Singleton.hpp"
#include <entt.hpp>
#include <glfw.hpp>
#include <glm.hpp>
#include <iostream>
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

    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE);
    window = glfwCreateWindow(640, 480, "a GLFW Window", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSwapInterval(0);

    if (glewInit())
      std::terminate();

    glfwSetWindowUserPointer(window, (void *)this);
    glfwSetKeyCallback(window, _glfw_key_callback);
    glfwSetCursorPosCallback(window, _glfw_cursor_position_callback);

    is_alive = true;

    auto &mesh = graphics::BoxMesh;

    glEnable(GL_MULTISAMPLE);
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEPTH_TEST);
    glDebugMessageCallback(_opengl_message_callback, nullptr);

    const char debug_handler_running_report[] = "Debug message handler running";
    glDebugMessageInsert(GL_DEBUG_SOURCE_THIRD_PARTY, GL_DEBUG_TYPE_OTHER, 11,
                         GL_DEBUG_SEVERITY_NOTIFICATION, 14,
                         &debug_handler_running_report[0]);
  }

  void tick_begin() {
    if (!is_alive) {
      glfwDestroyWindow(window);
      window = nullptr;
      return;
    }

    if (glfwWindowShouldClose(window))
      on_should_close(*this);
  }

  void tick_end() {
    glfwSwapBuffers(window);
    base::Clock::instance().frame_count++;
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

  GLFWwindow *window = nullptr;

protected:
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

  static void GLAPIENTRY _opengl_message_callback(GLenum source, GLenum type,
                                                  GLuint id, GLenum severity,
                                                  GLsizei length,
                                                  const GLchar *message,
                                                  const void *userParam) {
    fprintf(stderr,
            "GL CALLBACK: %s type = 0x%x, severity = 0x%x, message = %s\n",
            (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""), type,
            severity, message);
  }
};

struct WindowBeginSystem : core::System {
  WindowBeginSystem() { this->contract.global_priority = 6; }

  virtual void tick() override { Window::instance().tick_begin(); }
};

struct WindowEndSystem : core::System {
  WindowEndSystem() { this->contract.global_priority = -6; }

  virtual void tick() override { Window::instance().tick_end(); }
};

} // namespace geare::windowing

#endif
