#pragma once

#include <string>

#include <magic_enum/magic_enum.hpp>

#include <nanobind/nanobind.h>
#include <nanobind/stl/shared_ptr.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>
#include <nanobind/stl/pair.h>

// Convenience macros for binding.
#include <easybind/module/ns_module.hpp>

namespace easybind {

template <typename E>
nanobind::enum_<E> enum_(nanobind::module_& m, const char* name = nullptr) {
  std::string enum_name = name ? std::string(name)
                               : std::string(::magic_enum::enum_type_name<E>());
  auto e = nanobind::enum_<E>(m, enum_name.c_str());
  for (const auto& entry : ::magic_enum::enum_entries<E>()) {
    e.value(entry.second.data(), entry.first);
  }
  e.export_values();
  return e;
}

}  // namespace easybind