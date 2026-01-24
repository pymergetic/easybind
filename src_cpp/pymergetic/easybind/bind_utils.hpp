#pragma once

#include <nanobind/nanobind.h>

#include <pymergetic/easybind/asyncio.hpp>
#include <pymergetic/easybind/enum_utils.hpp>
#include <pymergetic/easybind/registry.hpp>

#define EASYBIND_DETAIL_CONCAT_INNER(a, b) a##b
#define EASYBIND_DETAIL_CONCAT(a, b) EASYBIND_DETAIL_CONCAT_INNER(a, b)

// Usage: EASYBIND_REGISTER([](nanobind::module_& m) { ... });
#define EASYBIND_REGISTER(LAMBDA) \
  static ::easybind::AutoRegister EASYBIND_DETAIL_CONCAT(_easybind_reg_, __COUNTER__)(LAMBDA)

// Usage: EASYBIND_REGISTER_PACKAGE("pkg.name", [](nanobind::module_& m) { ... });
#define EASYBIND_REGISTER_PACKAGE(PACKAGE, LAMBDA) \
  static ::easybind::AutoRegister EASYBIND_DETAIL_CONCAT(_easybind_reg_, __COUNTER__)(PACKAGE, 0, LAMBDA)

// Usage: EASYBIND_REGISTER_PACKAGE_PRI("pkg.name", 10, [](nanobind::module_& m) { ... });
#define EASYBIND_REGISTER_PACKAGE_PRI(PACKAGE, PRIORITY, LAMBDA) \
  static ::easybind::AutoRegister EASYBIND_DETAIL_CONCAT(_easybind_reg_, __COUNTER__)(PACKAGE, PRIORITY, LAMBDA)

// Usage: EASYBIND_REGISTER_ENUM("pkg.name", EnumType);
#define EASYBIND_REGISTER_ENUM(PACKAGE, TYPE) \
  EASYBIND_REGISTER_PACKAGE_PRI(PACKAGE, -10, [](nanobind::module_& m) { ::easybind::enums::bind<TYPE>(m, #TYPE); })

// Compatibility alias for PY_* naming.
#define PY_REGISTER EASYBIND_REGISTER
