#include <iostream>
#include <syncstream>

#include "./geare/base.hpp"
#include "./geare/core.hpp"
#include "./geare/graphics.hpp"
#include "./geare/utils.hpp"
#include "./geare/windowing.hpp"

using namespace geare::windowing;
using namespace geare::core;
using namespace geare::graphics;
using namespace geare::base;
using namespace geare::utils;

struct SpinnerSystem : StaticSystem<Transform, MeshRenderer> {
  virtual void tick(view_t &view) override {
    for (auto &entry : view) {
      auto &transform = view.get<Transform>(entry);
      transform.rotation.y +=
          Clock::instance().delta_time * pow(-1, (int)entry);
      transform.rotation.x += Clock::instance().delta_time / 2;
      transform.scale = glm::one<glm::vec3>() * .5f *
                        (sinf((float)Clock::instance().global_time) + 1) / 2.f;

      transform.position = glm::vec3(
          sinf(pow(.5f, (int)entry) * 4 * (float)Clock::instance().global_time),
          cosf(pow(.5f, (int)entry) * 4 * (float)Clock::instance().global_time),
          -7);
    }
  }
};

static int was_really_destroyed = false;

int main(void) {
  {
    auto arena = Arena<>();

    struct NonTriviallyDestructible {
      virtual ~NonTriviallyDestructible() { was_really_destroyed++; }
    };

    static_assert(!std::is_trivially_destructible_v<NonTriviallyDestructible>);

    auto bytes = arena.allocate_raw(4);
    auto throwaway = arena.allocate<NonTriviallyDestructible>();

    arena.clear();

    std::cout << std::boolalpha << was_really_destroyed << std::endl;
  }

  auto world = World();
  auto &scheduler = world.scheduler;

  auto &root_scene = world.active_scene;
  auto some_entity = root_scene.create();
  auto another_entity = root_scene.create();
  auto camera_entity = root_scene.create();

  root_scene.emplace<MeshRenderer>(some_entity, MeshRenderer());
  root_scene.emplace<Transform>(some_entity, Transform());

  root_scene.emplace<MeshRenderer>(another_entity, MeshRenderer());
  root_scene.emplace<Transform>(another_entity, Transform());

  root_scene.emplace<PerspectiveCamera>(camera_entity, PerspectiveCamera());
  root_scene.emplace<Transform>(camera_entity, Transform());

  root_scene.get<Transform>(some_entity).position = glm::vec3(0, 1, -6);
  root_scene.get<Transform>(another_entity).position = glm::vec3(0, -1, -6);
  root_scene.get<Transform>(camera_entity).position = glm::vec3(0, -6, 0);
  root_scene.get<Transform>(camera_entity).rotation = glm::vec3(0, 0, 1.67f);

  auto &window = Window::instance();

  scheduler.add_system(new ClockSystem());
  scheduler.add_system(new WindowBeginSystem());
  scheduler.add_system(new WindowEndSystem());
  scheduler.add_system(new GeometryCollectionSystem());
  scheduler.add_system(new TransformRefresherSystem());
  scheduler.add_system(new PerFrameMeshTransformGenerator());
  scheduler.add_system(new PerspectiveCameraSystem());

  Inputs::instance().register_keycode('X');
  scheduler.add_system(new FunctionSystem([]() {
    if (Inputs::instance().is_key_down('X')) {
      std::cout << "X key pressed!" << std::endl;
    } else if (Inputs::instance().is_key_up('X')) {
      std::cout << "X key released!" << std::endl;
    } else if (Inputs::instance().is_key_held('X')) {
      std::cout << "X key held!" << std::endl;
    }
  }));

  scheduler.add_system(new SpinnerSystem());
  scheduler.add_system(new InputSystem());

  window.show();

  window.on_should_close.connect([](const void *_, Window &window) {
    std::cout << "Should close triggered." << std::endl;
    window.close();
  });

  window.on_key_press.connect(
      [](const void *_, int key, int scancode, int action, int mods) {
        _glfw_keypress_adapter(key, scancode, action, mods);
      });

  while (window.is_alive) {
    scheduler.tick(world.active_scene);
  }

  return 0;
}
