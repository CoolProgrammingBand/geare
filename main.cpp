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

  auto task_factory = [&]() -> Task {
    static char id = 'a';
    return [](int i, int id, Executor &executor) -> Task {
      for (;;) {
        log_dbg((char)id, " did work and now has ", --i, " work left");

        if (i > 0) {
          // TODO: automatic component release
          auto view = co_await executor.get_components<A, B>();
          co_await std::suspend_always();
        } else
          break;
      };

      log_dbg((char)id, " is done!");
      co_return;
    }(4, id++, world.executor);
  };

  world.executor.enqueue_immediate_task(task_factory());
  world.executor.enqueue_immediate_task(task_factory());
  world.executor.enqueue_immediate_task(task_factory());
  world.executor.tick();

  return 0;
}
