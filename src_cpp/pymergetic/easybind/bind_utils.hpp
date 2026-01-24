#pragma once

#include <nanobind/nanobind.h>

#include <pymergetic/easybind/asyncio.hpp>
#include <pymergetic/easybind/asyncio_asio.hpp>
#include <pymergetic/easybind/describe.hpp>
#include <pymergetic/easybind/enum_utils.hpp>
#include <pymergetic/easybind/registry.hpp>

#define EASYBIND_DETAIL_CONCAT_INNER(a, b) a##b
#define EASYBIND_DETAIL_CONCAT(a, b) EASYBIND_DETAIL_CONCAT_INNER(a, b)

// Usage: EASYBIND_PACKAGE("pkg.name");
#define EASYBIND_PACKAGE(PACKAGE)                                                                 \
  namespace {                                                                                      \
  constexpr const char* EASYBIND_DETAIL_PACKAGE = PACKAGE;                                         \
  }

// Usage: EASYBIND_REGISTER([](nanobind::module_& m) { ... });
#define EASYBIND_REGISTER(LAMBDA) \
  static ::easybind::AutoRegister EASYBIND_DETAIL_CONCAT(_easybind_reg_, __COUNTER__)(LAMBDA)

// Usage: EASYBIND_REGISTER_PACKAGE("pkg.name", [](nanobind::module_& m) { ... });
#define EASYBIND_REGISTER_PACKAGE(PACKAGE, LAMBDA) \
  static ::easybind::AutoRegister EASYBIND_DETAIL_CONCAT(_easybind_reg_, __COUNTER__)(PACKAGE, 0, LAMBDA)

// Usage: EASYBIND_REGISTER_PACKAGE_PRI("pkg.name", 10, [](nanobind::module_& m) { ... });
#define EASYBIND_REGISTER_PACKAGE_PRI(PACKAGE, PRIORITY, LAMBDA) \
  static ::easybind::AutoRegister EASYBIND_DETAIL_CONCAT(_easybind_reg_, __COUNTER__)(PACKAGE, PRIORITY, LAMBDA)

// Usage: EASYBIND_REGISTER_ENUM(EnumType);
#define EASYBIND_REGISTER_ENUM(TYPE)                                                              \
  EASYBIND_REGISTER_PACKAGE_PRI(EASYBIND_DETAIL_PACKAGE, -10, [](nanobind::module_& m) {          \
    ::easybind::enums::bind<TYPE>(m, #TYPE);                                                      \
  })

// Usage: EASYBIND_REGISTER_FUNC(func, nb::arg("x") = 1, ...);
#define EASYBIND_REGISTER_FUNC(FUNC, ...)                                                         \
  EASYBIND_REGISTER_PACKAGE(EASYBIND_DETAIL_PACKAGE, [](nanobind::module_& m) {                   \
    m.def(#FUNC, &FUNC __VA_OPT__(, ) __VA_ARGS__);                                                \
  })

// Usage: EASYBIND_REGISTER_ASYNC(func, nb::arg("x") = 1, ...);
#define EASYBIND_REGISTER_ASYNC(FUNC, ...)                                                        \
  EASYBIND_REGISTER_PACKAGE(EASYBIND_DETAIL_PACKAGE, [](nanobind::module_& m) {                   \
    m.def(#FUNC, &::easybind::asyncio::AsyncReturnWrapper<FUNC>::call __VA_OPT__(, ) __VA_ARGS__); \
  })

// Usage: EASYBIND_REGISTER_CLASS(Type)
#define EASYBIND_REGISTER_CLASS(TYPE)                                                             \
  EASYBIND_REGISTER_PACKAGE(EASYBIND_DETAIL_PACKAGE, [](nanobind::module_& m) {                   \
    ::easybind::describe::bind_struct<TYPE>(m, #TYPE);                                            \
  })

// Usage: EASYBIND_REGISTER_CLASS_WITH(Type, [](auto& cls) { ... });
#define EASYBIND_REGISTER_CLASS_WITH(TYPE, FN)                                                     \
  EASYBIND_REGISTER_PACKAGE(EASYBIND_DETAIL_PACKAGE, [](nanobind::module_& m) {                   \
    ::easybind::describe::bind_struct<TYPE>(m, #TYPE, FN);                                         \
  })

// Usage: EASYBIND_REGISTER_CLASS_METHODS(Type, EASYBIND_DEF_METHOD(...), ...);
#define EASYBIND_REGISTER_CLASS_METHODS(TYPE, ...)                                                 \
  EASYBIND_REGISTER_PACKAGE(EASYBIND_DETAIL_PACKAGE, [](nanobind::module_& m) {                   \
    ::easybind::describe::bind_struct<TYPE>(m, #TYPE, [](auto& cls) {                              \
      __VA_ARGS__                                                                                 \
    });                                                                                           \
  })

// Compatibility alias for PY_* naming.
#define PY_REGISTER EASYBIND_REGISTER
