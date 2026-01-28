#include <nanobind/nanobind.h>

#include <pymergetic/easybind/module/hooks_nb.hpp>


//
// Extension-only helper implementation for nanobind ApplyHooks.
//


namespace pymergetic::easybind::module {


ApplyHooks make_nanobind_apply_hooks() {
  return ApplyHooks{
      [](nanobind::module_& module, const char* name) {
        return nanobind::hasattr(module, name);
      },
      [](nanobind::module_& module, const char* name) {
        return nanobind::borrow<nanobind::module_>(module.attr(name));
      },
      [](nanobind::module_& module, const char* name) {
        return module.def_submodule(name);
      },
  };
}


}  // namespace pymergetic::easybind::module
