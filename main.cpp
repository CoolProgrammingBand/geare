#include <coroutine>
#include <deque>
#include <iostream>
#include <syncstream>

#include "./geare/base.hpp"
#include "./geare/core.hpp"
#include "./geare/utils.hpp"

using namespace geare::core;
using namespace geare::base;
using namespace geare::utils;

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

struct Executor {
  void enqueue_immediate_task(Task &&task) { tasks.push_back(task); }
  void enqueue_delayed_task(Task &&task) { future_tasks.push_back(task); }

  void tick() {
    while (!tasks.empty())
      this->step();
    std::swap(future_tasks, tasks);
  }

  void step() {
    Task task = std::move(tasks.front());
    tasks.pop_front();
    if (!task.done()) {
      task.resume();
      if (!task.done()) {
        tasks.push_back(task);
      }
    }
  }

  std::deque<Task> tasks;
  std::deque<Task> future_tasks;
};

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
