#include <coroutine>
#include <deque>
#include <iostream>
#include <syncstream>

#include "./geare/core/Executor.hpp"
#include "./geare/core/Logger.hpp"
#include "./geare/core/World.hpp"
#include "./geare/utils.hpp"

using namespace geare::utils;
using namespace geare::core;
using namespace geare;

struct A {};
struct B {};
struct C {};

int main(void) {
  auto world = World();

  auto task_factory = [&](std::string_view task_name) -> Task {
    static char id = 'a';
    auto coro = [](int i, Executor &executor) -> Task {
      auto view = co_await executor.get_components<A, const B>();

      for (;;) {
        log_dbg("Did work and now has ", --i, " work left");

        if (i > 0) {
          {}
          co_await std::suspend_always();
        } else
          break;
      };

      log_dbg("Done!");
      co_return;
    }(4, world.executor);
    coro.promise().set_name(task_name);
    return coro;
  };

  world.executor.enqueue_immediate_task(task_factory("alpha"));
  world.executor.enqueue_immediate_task(task_factory("beta"));
  world.executor.enqueue_immediate_task(task_factory("gamma"));
  world.executor.tick();

  return 0;
}
