#ifndef _INCLUDE__GEARE__CORE__INPUTS_
#define _INCLUDE__GEARE__CORE__INPUTS_

#include "../utils/Singleton.hpp"
#include "System.hpp"
#include "entt.hpp"
#include "glfw.hpp"
#include <set>
#include <tuple>

namespace geare::core {

enum struct KeyState {
  None,
  Down,
  Held,
  Up,
};

struct Inputs : utils::Singleton<Inputs> {
  using KeyHandle = std::tuple<KeyState, entt::dispatcher>;

  void tick() {
    for (auto keycode : captured_keycodes) {
      auto &key_state = state[keycode];
      switch (key_state) {
      case KeyState::Down:
        key_state = KeyState::Held;
        break;
      case KeyState::Up:
        key_state = KeyState::None;
        break;
      default:
      case KeyState::None:
      case KeyState::Held:
        break;
      }
    }
  }

  inline void register_keycode(int keycode) {
    captured_keycodes.insert(keycode);
  }

  inline void handle_key_press(int keycode) {
    if (captured_keycodes.contains(keycode))
      state[keycode] = KeyState::Down;
  }

  inline void handle_key_release(int keycode) {
    if (captured_keycodes.contains(keycode))
      state[keycode] = KeyState::Up;
  }

  inline bool is_key_down(int keycode) {
    return state[keycode] == KeyState::Down;
  }

  inline bool is_key_up(int keycode) { return state[keycode] == KeyState::Up; }

  inline bool is_key_held(int keycode) {
    return state[keycode] == KeyState::Held;
  }

  std::set<int> captured_keycodes;
  std::map<int, KeyState> state;

  // TODO: trigger events when certain inputs are triggered
};

struct InputSystem : core::System {
  virtual void tick() override { Inputs::instance().tick(); }
};

static void _glfw_keypress_adapter(int keycode, int scancode, int action,
                                   int mods) {
  if (action == GLFW_PRESS) {
    Inputs::instance().handle_key_press(keycode);
  } else if (action == GLFW_RELEASE) {
    Inputs::instance().handle_key_release(keycode);
  }
}

} // namespace geare::core

#endif
