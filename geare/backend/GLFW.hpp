#ifndef _INCLUDE__GEARE__BACKENDS__GLFW_
#define _INCLUDE__GEARE__BACKENDS__GLFW_

#include "../core/Executor.hpp"

#include <glfw.hpp>

namespace geare::backend {

auto init_glfw() -> core::Executor::Task {
  if (!glfwInit())
    std::terminate();
  co_return;
}

} // namespace geare::backend

#endif
