#ifndef _INCLUDE__GEARE__CORE__EXECUTOR_
#define _INCLUDE__GEARE__CORE__EXECUTOR_

#include <coroutine>
#include <deque>
#include <map>
#include <optional>
#include <string>

#include "./AdvancedRegistry.hpp"
#include "./Logger.hpp"

#include <entt.hpp>

namespace geare::core {

struct task_promise_t;

struct Task : std::coroutine_handle<task_promise_t> {
  using promise_type = task_promise_t;
};

struct task_promise_t {
  std::optional<std::string> task_name;

  void set_name(std::string_view name) { this->task_name = name; }

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
      auto &maybe_task_name = task.promise().task_name;
      log_begin_ctx(maybe_task_name.value_or("none"));
      task.resume();
      log_end_ctx();
    }
  }

  template <typename... Ts> struct AwaitForComponents {
    Executor *executor;

    AwaitForComponents(Executor *executor) : executor(executor) {}

    bool await_ready() { return false; }

    auto await_resume() { return executor->registry->get_components<Ts...>(); }

    void await_suspend(std::coroutine_handle<task_promise_t> handle) {
      log_dbg("Enqueued into the component waiting list");

      // TODO: just a stub to test if this approach even works
      executor->waiting_on_components.push_back(std::make_pair(
          handle.promise().get_return_object(),
          std::vector<ComponentAccess>(multicomponent_access<Ts...>.begin(),
                                       multicomponent_access<Ts...>.end())));
    }
  };

  template <typename... Ts> auto get_components() -> AwaitForComponents<Ts...> {
    return AwaitForComponents<Ts...>(this);
  }

  std::deque<std::pair<Task, std::vector<ComponentAccess>>>
      waiting_on_components;

  std::deque<Task> tasks;
  std::deque<Task> future_tasks;
};

} // namespace geare::core

#endif
