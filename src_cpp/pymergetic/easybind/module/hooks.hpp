#pragma once

#include <nanobind/nanobind.h>


//
// Core (pure C++) hook registry (no nanobind calls here).
//


namespace pymergetic::easybind::module {


struct ApplyHooks {
  bool (*has_child)(nanobind::module_& module, const char* name);
  nanobind::module_ (*get_child)(nanobind::module_& module, const char* name);
  nanobind::module_ (*def_child)(nanobind::module_& module, const char* name);

  static void set(const ApplyHooks& hooks);
  static const ApplyHooks* get();
};


}  // namespace pymergetic::easybind::module
