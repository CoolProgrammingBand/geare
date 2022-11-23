#ifndef _INCLUDE__GEARE__CORE__SCHEDULER_
#define _INCLUDE__GEARE__CORE__SCHEDULER_

#include <coroutine>
#include <deque>

namespace geare::core {

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

} // namespace geare::core

#endif
