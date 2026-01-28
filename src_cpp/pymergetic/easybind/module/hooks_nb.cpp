#include <nanobind/nanobind.h>

#include <pymergetic/easybind/module/hooks_nb.hpp>

#include <stdexcept>


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

void apply_init(ModuleNode* init_node, nanobind::module_& m) {
  ApplyHooks::set(make_nanobind_apply_hooks());

  if (init_node) {
    init_node->apply(m);
  } else {
    throw std::runtime_error("easybind module node not initialized");
  }
}


}  // namespace pymergetic::easybind::module
