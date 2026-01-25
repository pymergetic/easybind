#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include <string>

#include <pymergetic/easybind/asyncio_asio.hpp>
#include <pymergetic/easybind/bind_utils.hpp>

namespace easybind {

inline auto arg(const char* name) {
  return nanobind::arg(name);
}

}  // namespace easybind

// Usage: EASYBIND_MODULE("pkg.name", module_name, m)
#define EASYBIND_MODULE(PACKAGE, MODULE_NAME, MODULE_VAR)                                         \
  NB_MODULE(MODULE_NAME, MODULE_VAR) {                                                            \
    MODULE_VAR.doc() = std::string(PACKAGE) + " module";                                           \
    ::easybind::Registry::get().apply_all_for(PACKAGE, MODULE_VAR);                               \
  }

// Usage: EASYBIND_MODULE_PACKAGE(module_name)
#define EASYBIND_MODULE_PACKAGE(MODULE_NAME)                                                      \
  EASYBIND_MODULE(::easybind::detail::k_package, MODULE_NAME, m)
