#pragma once

#include <pymergetic/easybind/module/node.hpp>


//
// Extension-side bindings for easybind.module.
//

namespace pymergetic::easybind::module {


void __init_bind__(nanobind::module_& m);
extern ModuleNode* __init__;


}  // namespace pymergetic::easybind::module
