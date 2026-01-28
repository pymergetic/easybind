#pragma once

#include <pymergetic/easybind/module/hooks.hpp>


//
// Extension-only helper to build nanobind ApplyHooks.
//


namespace pymergetic::easybind::module {


ApplyHooks make_nanobind_apply_hooks();


}  // namespace pymergetic::easybind::module
