#include <nanobind/nanobind.h>

#include <pymergetic/easybind/registry.hpp>

namespace nb = nanobind;

NB_MODULE(_internal, m) {
  m.doc() = "easybind registry bootstrap module";
  easybind::Registry::get().apply_all(m);
}
