#ifndef _INCLUDE__GEARE__CORE__EXECUTOR_
#define _INCLUDE__GEARE__CORE__EXECUTOR_

#include <coroutine>
#include <deque>
#include <map>
#include <optional>
#include <span>
#include <string>

#include "./AdvancedRegistry.hpp"
#include "./Logger.hpp"

#include <entt.hpp>

namespace geare::core {

struct Executor {
  AdvancedRegistry *registry;

  Executor(AdvancedRegistry *registry) : registry(registry) {}

  struct TaskPromise;

  struct Task : std::coroutine_handle<TaskPromise> {
    using promise_type = TaskPromise;
  };

  struct TaskPromise {
    std::optional<std::string> task_name;

    void set_name(std::string_view name) { this->task_name = name; }

    Task get_return_object() { return {Task::from_promise(*this)}; }
    std::suspend_always initial_suspend() noexcept { return {}; }
    std::suspend_always final_suspend() noexcept { return {}; }

    void return_void() {}

    void unhandled_exception() {
      std::exception_ptr ex = std::current_exception();

      this->get_return_object().destroy();
      try {
        std::rethrow_exception(ex);
      } catch (const std::exception &e) {
        log_err("Unhandled exception in a task, propagating ", e.what());
        std::rethrow_exception(ex);
      }
    }
  };

  void schedule(Task &&task) { tasks.push_back(task); }
  void schedule_next(Task &&task) { future_tasks.push_back(task); }

  Arena<> tick_arena;
  Arena<> step_arena;

  void tick() {
    while (waiting_on_components.size() + tasks.size() > 0) {
      while (!tasks.empty())
        this->step();
      _try_reschedule_waiting_tasks();
    }

    std::swap(future_tasks, tasks);
    tick_arena.clear();
  }

  void step() {
    Task task = std::move(tasks.front());
    tasks.pop_front();
    if (!task.done()) {
      auto &maybe_task_name = task.promise().task_name;
      log_begin_ctx(maybe_task_name.value_or("none"));
      task.resume();
      step_arena.clear();
      log_end_ctx();
    }
  }

  void _try_reschedule_waiting_tasks() {
    auto currently_waiting = std::move(waiting_on_components);

    while (!currently_waiting.empty()) {
      auto [task, awaiting_components] =
          std::pair(std::move(currently_waiting.front()));
      currently_waiting.pop_front();

      log_begin_ctx(task.promise().task_name.value_or("none"));

      auto borrows = awaiting_components->get_borrows();
      if (this->registry->can_borrow(borrows)) {
        log_dbg("Got components!");
        awaiting_components->_resolve();
        tasks.push_front(task);
      } else {
        log_dbg("Can't get components, deferring");
        waiting_on_components.push_back(
            std::make_pair(task, awaiting_components));
      }

      log_end_ctx();
    }
  }

  struct AwaitExecutor : std::suspend_always {
    Executor *executor;

    AwaitExecutor(Executor *executor) : executor(executor) {}
  };

  struct AwaitDefer : AwaitExecutor {
    void await_suspend(std::coroutine_handle<TaskPromise> handle) {
      executor->schedule(handle.promise().get_return_object());
    }
  };

  auto defer() -> AwaitDefer { return AwaitDefer{this}; }

  struct AbstractAwaitComponents : AwaitExecutor {
    using AwaitExecutor::AwaitExecutor;
    virtual std::span<const ComponentBorrowDescriptor> get_borrows() const = 0;
    virtual void _resolve() = 0;
  };

  template <typename... Ts> struct AwaitComponents : AbstractAwaitComponents {
    using AbstractAwaitComponents::AbstractAwaitComponents;

    std::optional<AdvancedRegistry::SimpleView<Ts...>> resolved_view =
        std::nullopt;

    virtual std::span<const ComponentBorrowDescriptor>
    get_borrows() const override {
      return std::span(multicomponent_access<Ts...>);
    }

    bool await_ready() {
      bool is_ready = executor->registry->can_borrow(this->get_borrows());
      if (is_ready)
        _resolve();
      return is_ready;
    }

    auto await_resume() {
      if (!resolved_view)
        throw std::logic_error("Attempt to access an unresolved view");
      return std::move(resolved_view.value());
    }

    void await_suspend(std::coroutine_handle<TaskPromise> handle) {
      log_dbg("Enqueued into the component waiting list");
      executor->waiting_on_components.push_back(
          std::make_pair(handle.promise().get_return_object(), this));
    }

    void _resolve() override final {
      resolved_view = executor->registry->get_components<Ts...>();
    }
  };

  template <typename... Ts> auto get_components() -> AwaitComponents<Ts...> {
    return AwaitComponents<Ts...>(this);
  }

  std::deque<std::pair<Task, AbstractAwaitComponents *>> waiting_on_components;
  std::deque<Task> tasks;
  std::deque<Task> future_tasks;
};

} // namespace geare::core

#endif
