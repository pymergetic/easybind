#pragma once

#include <pymergetic/easybind/module/hooks.hpp>
#include <pymergetic/easybind/module/node.hpp>


//
// Extension-only helper to build nanobind ApplyHooks.
//


namespace pymergetic::easybind::module {


ApplyHooks make_nanobind_apply_hooks();
void apply_init(ModuleNode* init_node, nanobind::module_& m);


}  // namespace pymergetic::easybind::module
