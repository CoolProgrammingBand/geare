#ifndef _INCLUDE__GEARE__CORE__SCHEDULER_
#define _INCLUDE__GEARE__CORE__SCHEDULER_

#include <coroutine>
#include <deque>
#include <map>

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

enum struct ComponentAccessType : char {
  Const,
  Mut,
};

using UniqueComponentIdentifier = entt::id_type;

using ComponentAccess =
    std::pair<UniqueComponentIdentifier, ComponentAccessType>;

template <typename... Ts>
using get_view_t =
    entt::basic_view<entt::entity, entt::get_t<Ts...>, entt::exclude_t<>, void>;

struct Executor;

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
    // This can be easily optimized away into a static
    std::vector<ComponentAccess> accesses;

    AwaitForComponents(Executor *executor)
        : executor(executor),
          accesses({{entt::type_id<Ts>().hash(),
                     std::is_const_v<Ts> ? ComponentAccessType::Const
                                         : ComponentAccessType::Mut}...}) {}

    bool await_ready() {
      log_dbg("| Attempt to get components: ", entt::type_id<Ts>().name()...);

      bool ready = true;
      for (auto &access : accesses)
        ready &= executor->is_component_available(access);

      if (ready) {
        for (auto &access : accesses)
          executor->use_component(access);

        log_dbg("|- Got components!");
      } else
        log_dbg("|- Failed to get components!");

      return ready;
    }

    auto await_resume() { return executor->component_registry->view<Ts...>(); }

    void await_suspend(std::coroutine_handle<task_promise_t> handle) {}
  };

  template <typename... Ts> auto get_components() -> AwaitForComponents<Ts...> {
    return AwaitForComponents<Ts...>(this);
  }

  bool is_component_available(ComponentAccess access) {
    auto [id, access_as] = access;
    return !mut_map[id] &&
           ((access_as == ComponentAccessType::Mut) <= !const_map[id]);
  }

  void use_component(ComponentAccess access) {
    auto [id, access_as] = access;
    if (access_as == ComponentAccessType::Mut)
      mut_map[id] = true;
    else
      const_map[id]++;
  }

  void release_component(ComponentAccess access) {
    auto [id, access_as] = access;
    if (access_as == ComponentAccessType::Mut)
      mut_map[id] = false;
    else
      const_map[id]--;
  }

  std::map<UniqueComponentIdentifier, bool> mut_map;
  std::map<UniqueComponentIdentifier, unsigned> const_map;

  std::deque<Task> tasks;
  std::deque<Task> future_tasks;
};

} // namespace geare::core

#endif
