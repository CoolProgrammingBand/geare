#ifndef _INCLUDE__GEARE__CORE__SCHEDULER_
#define _INCLUDE__GEARE__CORE__SCHEDULER_

#include <coroutine>
#include <deque>
#include <map>

#include <entt.hpp>

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

enum struct ComponentAccessType : char {
  Const,
  Mut,
};

struct UniqueComponentIdentifier {
  entt::id_type id;
};

using ComponentAccess =
    std::pair<UniqueComponentIdentifier, ComponentAccessType>;

struct Executor {
  entt::registry *component_registry;
  Executor(entt::registry *component_registry)
      : component_registry(component_registry) {}

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

  template <typename... Ts> struct AwaitForComponents {
    Executor *executor;

    AwaitForComponents(Executor *executor) : executor(executor) {}

    bool await_ready() { return false; }

    auto await_resume() { return executor->component_registry->view<Ts...>(); }

    void await_suspend(std::coroutine_handle<task_promise_t> handle) {}
  };

  template <typename... Ts> auto get_components() -> AwaitForComponents<Ts...> {
    return AwaitForComponents<Ts...>(this);
  }

  std::deque<Task> tasks;
  std::deque<Task> future_tasks;
};

} // namespace geare::core

#endif
