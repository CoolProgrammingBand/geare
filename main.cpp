#include <coroutine>
#include <iostream>
#include <syncstream>

#include "./geare/base.hpp"
#include "./geare/core.hpp"
#include "./geare/utils.hpp"

using namespace geare::core;
using namespace geare::base;
using namespace geare::utils;

struct Executor {};

struct task_promise_t;

struct Task : std::coroutine_handle<task_promise_t> {
  using promise_type = task_promise_t;
};

struct task_promise_t {
  Task get_return_object() { return {Task::from_promise(*this)}; }
  std::suspend_always initial_suspend() noexcept { return {}; }
  std::suspend_always final_suspend() noexcept { return {}; }

  void return_void() {}
  void unhandled_exception() {}
};

int main(void) {
  auto executor = Executor();

  Task task = [](int i, Executor &executor) -> Task {
    while (i > 0) {
      std::cout << i-- << std::endl;
      co_await std::suspend_always();
    }
    std::cout << "Done!" << std::endl;
    co_return;
  }(4, executor);

  while (!task.done()) {
    task.resume();
  }

  task.destroy();
  return 0;
}
