#include <coroutine>
#include <deque>
#include <iostream>
#include <syncstream>

#include "./geare/backend/GLFW.hpp"
#include "./geare/base/Clock.hpp"
#include "./geare/core/Executor.hpp"
#include "./geare/core/Logger.hpp"
#include "./geare/core/World.hpp"
#include "./geare/utils.hpp"

using namespace geare::utils;
using namespace geare::core;
using namespace geare::backend;
using namespace geare;

struct A {};
struct B {};
struct C {};

int main(void) {
  auto world = World();

  auto task_factory = [&](std::string_view task_name) -> Executor::Task {
    auto coro = [](int i, Executor &executor) -> Executor::Task {
      auto view = co_await executor.get_components<A, const B>();
      for (; i > 0; i--) {
        log_dbg("Did work and now has ", i, " work left");
        co_await executor.defer();
      };
      log_dbg("Done!");
      view.release();
      co_return;
    }(4, world.executor);

    coro.promise().set_name(task_name);
    return coro;
  };

  world.executor.schedule(init_glfw());
  world.executor.schedule(task_factory("alpha"));
  world.executor.schedule(task_factory("beta"));
  world.executor.schedule(task_factory("gamma"));

  Arena<> systems_arena;

  world.executor.systems.push_back(systems_arena.allocate<base::ClockSystem>());

  for (;;) {
    world.executor.tick();
  }

  return 0;
}
