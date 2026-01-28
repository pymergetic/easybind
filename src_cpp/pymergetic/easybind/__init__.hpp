#pragma once

#include <pymergetic/easybind/module/__init__.hpp>


namespace pymergetic::easybind {


void __init_bind__(nanobind::module_& m);
extern module::ModuleNode* __init__;


}  // namespace pymergetic::easybind
