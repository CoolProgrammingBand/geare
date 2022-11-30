#include <coroutine>
#include <deque>
#include <iostream>
#include <syncstream>

#include "./geare/backend/GLFW.hpp"
#include "./geare/base/Clock.hpp"
#include "./geare/core/Executor.hpp"
#include "./geare/core/Logger.hpp"
#include "./geare/core/World.hpp"
#include "./geare/graphics/MeshRenderer.hpp"
#include "./geare/graphics/PerspectiveCamera.hpp"
#include "./geare/utils.hpp"
#include "./geare/windowing/Window.hpp"

using namespace geare::utils;
using namespace geare::core;
using namespace geare::backend;
using namespace geare;

struct A {};
struct B {};
struct C {};

int main(void) {
  auto world = World();
  auto &registry = world.registry;

  auto cube1 = registry.create();
  auto cube2 = registry.create();
  auto camera = registry.create();

  registry.emplace<base::Transform>(cube1);
  registry.emplace<base::Transform>(cube2);
  registry.emplace<base::Transform>(camera);

  registry.emplace<graphics::MeshRenderer>(cube1);
  registry.emplace<graphics::MeshRenderer>(cube2);
  registry.emplace<graphics::PerspectiveCamera>(camera);

  registry.get<base::Transform>(camera).position.z -= 6;
  registry.get<graphics::PerspectiveCamera>(camera).main = true;

  Arena<> systems_arena;

  world.executor.systems.push_back(systems_arena.allocate<base::ClockSystem>());
  world.executor.systems.push_back(
      systems_arena.allocate<windowing::WindowSystem>());
  world.executor.systems.push_back(
      systems_arena.allocate<graphics::PerspectiveCameraSystem>());

  windowing::Window::instance().show();

  while (windowing::Window::instance().is_alive) {
    world.executor.tick();
  }

  return 0;
}
