#pragma once

#include <pymergetic/easybind/module/node.hpp>


namespace pymergetic::easybind {


extern module::ModuleNode* __init__;
void __init_bind__(nanobind::module_& m);


}  // namespace pymergetic::easybind
