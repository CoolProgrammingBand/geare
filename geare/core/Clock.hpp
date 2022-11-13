#ifndef _INCLUDE__GEARE__CORE__CLOCK_
#define _INCLUDE__GEARE__CORE__CLOCK_

#include "../utils/Singleton.hpp"

namespace geare::core {

struct Clock : utils::Singleton<Clock> {
  double delta_time;
  double fixed_timestep;
  double global_time;
  unsigned long frame_count;
};

} // namespace geare::core

#endif
