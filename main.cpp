#include <coroutine>
#include <deque>
#include <iostream>
#include <syncstream>

#include "./geare/core/Scheduler.hpp"
#include "./geare/utils.hpp"

using namespace geare::utils;
using namespace geare::core;

int main(void) {
  auto executor = Executor();

  auto task_factory = [&]() -> Task {
    static int id = 0;
    return [](int i, int id, Executor &executor) -> Task {
      for (;;) {
        i--;
        std::cout << "coro" << id << " did work and now has " << i
                  << " work left" << std::endl;
        if (i > 0)
          co_await std::suspend_always();
        else
          break;
      };

      std::cout << "coro" << id << " is done!" << std::endl;
      co_return;
    }(4, id++, executor);
  };

  executor.enqueue_immediate_task(std::move(task_factory()));
  executor.enqueue_immediate_task(std::move(task_factory()));
  executor.enqueue_immediate_task(std::move(task_factory()));
  executor.tick();

  return 0;
}
