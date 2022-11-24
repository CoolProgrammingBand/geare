#ifndef _INCLUDE__GEARE__CORE__EXECUTOR_
#define _INCLUDE__GEARE__CORE__EXECUTOR_

#include <coroutine>
#include <deque>
#include <map>

#include "./AdvancedRegistry.hpp"
#include "./Logger.hpp"

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

struct Executor {
  AdvancedRegistry *registry;

  Executor(AdvancedRegistry *registry) : registry(registry) {}

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

    bool await_ready() {
      log_dbg("| Attempt to get components: ", entt::type_id<Ts>().name()...);

      bool ready = true;
      for (auto &access : multicomponent_access<Ts...>)
        ready &= executor->registry->can_access_component(access);

      if (ready) {
        log_dbg("|- Got components!");
      } else
        log_dbg("|- Failed to get components!");

      return ready;
    }

    auto await_resume() { return executor->registry->get_components<Ts...>(); }

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
