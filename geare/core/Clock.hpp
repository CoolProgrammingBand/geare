#ifndef _INCLUDE__GEARE__CORE__CLOCK_
#define _INCLUDE__GEARE__CORE__CLOCK_

#include "../utils/Singleton.hpp"
#include "System.hpp"

#include <iostream>
#include <GLFW/glfw3.h>

namespace geare::core {

struct Clock : utils::Singleton<Clock> {
  double delta_time;
  double fixed_timestep;
  double global_time;
  unsigned long frame_count;
};

struct ClockSystem : System {
  ClockSystem() {
    contract.call_only_on_main = false;
    contract.global_priority = INT32_MAX;
  }

  virtual void tick() override {
    auto &clock = core::Clock::instance();
    double current_tick_time = glfwGetTime();
    clock.global_time = current_tick_time;
    clock.delta_time = current_tick_time - last_tick_time;
    last_tick_time = current_tick_time;
    std::cout << clock.delta_time << std::endl;
  }

  double last_tick_time = 0;
};

} // namespace geare::core

#endif
