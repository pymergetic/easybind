#include <nanobind/nanobind.h>

#include <pymergetic/easybind/refresh.hpp>
#include <pymergetic/easybind/registry.hpp>

namespace easybind {

void refresh_bindings() {
  nanobind::module_ module = nanobind::module_::import_(EASYBIND_BOOTSTRAP_PACKAGE);
  refresh_registered_packages(module, EASYBIND_BOOTSTRAP_PACKAGE);
}

}  // namespace easybind
