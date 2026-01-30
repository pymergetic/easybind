#pragma once

#include <pymergetic/easybind/module/node.hpp>


//
// Extension-side bindings for easybind.module.
//

namespace pymergetic::easybind::module {


extern ModuleNode* __init__;
void __init_bind__(nanobind::module_& m);


}  // namespace pymergetic::easybind::module
