#pragma once

#include <nanobind/nanobind.h>

#include <magic_enum/magic_enum.hpp>

namespace easybind::enums {

template <typename E>
inline nanobind::enum_<E> bind(nanobind::module_& m, const char* name) {
  auto e = nanobind::enum_<E>(m, name);
  for (const auto& entry : ::magic_enum::enum_entries<E>()) {
    e.value(entry.second.data(), entry.first);
  }
  return e;
}

}  // namespace easybind::enums
