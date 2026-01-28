#pragma once

#include <pymergetic/easybind/module/__init__.hpp>
#include <pymergetic/easybind/bind.hpp>


namespace pymergetic::easybind {

    
void __init_bind__(nanobind::module_& m);
extern module::ModuleNode* __init__;


void apply_init(module::ModuleNode* init_node, nanobind::module_& m);


}  // namespace pymergetic::easybind
