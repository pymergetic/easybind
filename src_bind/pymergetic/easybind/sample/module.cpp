#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include <pymergetic/easybind/registry.hpp>

namespace nb = nanobind;

NB_MODULE(_sample, m) {
  m.doc() = "easybind sample module";
  easybind::Registry::get().apply_all_for("pymergetic.easybind.sample", m);
}
