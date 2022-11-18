#include <algorithm>
#include <set>
#include <vector>

using namespace std;

using ThreadID = unsigned;
using ComponentID = unsigned;

// is_main_threaded, priority, mut_components, const_components

struct System;

struct Thread {
  System *active_system = nullptr;
  ThreadID id;

  void assign_system(System *system) { active_system = system; }
};

struct System {
  bool is_main_threaded;
  int priority; // computed somewhere
  std::set<ComponentID> mut_components;
  std::set<ComponentID> const_components;
};

bool comp(const System &a, const System &b) {
  // is_main_threaded -> priority -> mut_components -> const_components
  if (a.is_main_threaded && b.is_main_threaded)
    return a.priority < b.priority; // check sign

  if (a.is_main_threaded)
    return false;
  if (b.is_main_threaded)
    return true;

  return a.priority < b.priority; // check sign
}

int main(void) {
  vector<System> systems;

  sort(systems.begin(), systems.end(), comp); //nlogn

  vector<Thread> threads = {{.id = 0}, {.id = 1}};

  set<ComponentID> cur_mut_components = systems[0].mut_components;
  set<ComponentID> cur_const_components = systems[0].const_components;
  int cur_sys_i = 0;
  threads[0].assign_system(&systems[0]);

  while (systems[cur_sys_i++].is_main_threaded)
    ;

  // Initial distribution of systems
  for (int thread_i = 1;
       thread_i < threads.size() && cur_sys_i < systems.size();) { //m

    bool isOk = true;
    for (auto &mut_comp : systems[cur_sys_i].mut_components) { // M
      auto is_used_as_const = cur_const_components.contains(mut_comp);
      auto is_used_as_mut = cur_mut_components.contains(mut_comp);
      isOk &= !(is_used_as_mut || is_used_as_const);
    }

    for (auto &const_comp : systems[cur_sys_i].const_components) // M
      isOk &= !cur_mut_components.contains(const_comp);

    if (!isOk) {
      // Skip to next system
      ++cur_sys_i;
    } else {
      threads[thread_i++].assign_system(&systems[cur_sys_i++]);
    }
  }

  return 0;
} //nlogn + m*MlogM*logM + O(1)
