#pragma once

#include <pymergetic/easybind/module/node.hpp>
#include <pymergetic/easybind/module/hooks_nb.hpp>


//
// Extension-side bindings for easybind.module.
//

namespace pymergetic::easybind::module {


void __init_bind__(nanobind::module_& m);
extern ModuleNode* __init__;


void set_package_path(nanobind::module_& m);


}  // namespace pymergetic::easybind::module
