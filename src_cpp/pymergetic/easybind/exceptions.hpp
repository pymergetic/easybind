#pragma once

#include <nanobind/nanobind.h>

namespace easybind::exceptions {

namespace nb = nanobind;

template <typename Exc>
inline void bind(nb::module_& m, const char* name) {
  nb::exception<Exc>(m, name);
}

inline nb::object import(const char* module_name, const char* exc_name) {
  nb::module_ mod = nb::module_::import_(module_name);
  return mod.attr(exc_name);
}

inline void export_alias(nb::module_& m, const char* module_name, const char* exc_name) {
  m.attr(exc_name) = import(module_name, exc_name);
}

}  // namespace easybind::exceptions
