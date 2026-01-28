#include <atomic>

#include <pymergetic/easybind/module/hooks.hpp>


//
// Core (pure C++) hook registry implementation.
//


namespace pymergetic::easybind::module {

ApplyHooks g_apply_hooks{};
std::atomic<bool> g_apply_hooks_set{false};


void ApplyHooks::set(const ApplyHooks& hooks) {
  g_apply_hooks = hooks;
  g_apply_hooks_set.store(true, std::memory_order_release);
}

const ApplyHooks* ApplyHooks::get() {
  return g_apply_hooks_set.load(std::memory_order_acquire) ? &g_apply_hooks : nullptr;
}


}  // namespace pymergetic::easybind::module
