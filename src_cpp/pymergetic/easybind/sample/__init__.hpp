#pragma once

#include <nanobind/nanobind.h>

#include <pymergetic/easybind/module/node.hpp>


namespace pymergetic::easybind::sample {


extern module::ModuleNode* __init__;
void __init_bind__(nanobind::module_& m);


}  // namespace pymergetic::easybind::sample
