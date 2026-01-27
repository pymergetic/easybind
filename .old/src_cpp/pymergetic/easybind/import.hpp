#pragma once

#include <nanobind/nanobind.h>

namespace easybind {

inline nanobind::module_ import_module(const char* name) {
  return nanobind::module_::import_(name);
}

}  // namespace easybind
