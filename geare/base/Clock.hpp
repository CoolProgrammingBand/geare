#ifndef _INCLUDE__GEARE__BASE__CLOCK_
#define _INCLUDE__GEARE__BASE__CLOCK_

#include "../core/Executor.hpp"
#include "../core/Logger.hpp"
#include "../utils/Singleton.hpp"

#include <glfw.hpp>

namespace geare::base {

struct Clock : utils::Singleton<Clock> {
  double delta_time;
  double fixed_timestep;
  double global_time;
  unsigned long tick_count;

  double tick_timer;
  unsigned long tps;
};

struct ClockSystem : core::Executor::System {
  ClockSystem() {}

  virtual auto task(core::Executor *exec) -> core::Executor::Task override {
    auto &clock = Clock::instance();

    log_begin_ctx("Clock");

    clock.tick_count++;
    double current_tick_time = glfwGetTime();
    clock.global_time = current_tick_time;
    clock.delta_time = current_tick_time - last_tick_time;
    last_tick_time = current_tick_time;

    if (clock.global_time - clock.tick_timer > 1) {
      clock.tps = clock.tick_count;
      clock.tick_count = 0;
      clock.tick_timer = clock.global_time;
      log_dbg("TPS: ", clock.tps);
    }

    log_end_ctx();
    co_return;
  }

  double last_tick_time = 0;
};

} // namespace geare::base

#endif
