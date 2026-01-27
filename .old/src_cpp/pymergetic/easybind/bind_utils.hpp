#pragma once

#include <nanobind/nanobind.h>

#include <tuple>
#include <utility>

#include <pymergetic/easybind/asyncio.hpp>
#include <pymergetic/easybind/asyncio_asio.hpp>
#include <pymergetic/easybind/describe.hpp>
#include <pymergetic/easybind/enum_utils.hpp>
#include <pymergetic/easybind/exceptions.hpp>
#include <pymergetic/easybind/registry.hpp>

// -----------------------------
// Internal helpers (do not use)
// -----------------------------
#define EASYBIND_DETAIL_CONCAT_INNER(a, b) a##b
#define EASYBIND_DETAIL_CONCAT(a, b) EASYBIND_DETAIL_CONCAT_INNER(a, b)

#ifndef EASYBIND_ENABLED
#define EASYBIND_ENABLED 1
#endif

#if EASYBIND_ENABLED

// -----------------------------
// Public binding API (use these)
// -----------------------------

namespace easybind::bind {

template <typename Fn>
inline AutoRegister global(Fn&& fn) {
  return AutoRegister(std::forward<Fn>(fn));
}

template <typename Fn>
inline AutoRegister package(const char* package, int priority, Fn&& fn) {
  return AutoRegister(std::string(package), priority, std::forward<Fn>(fn));
}

template <auto Func, typename... Args>
inline AutoRegister global_func(const char* name, Args&&... args) {
  return AutoRegister(
      [name, args = std::make_tuple(std::forward<Args>(args)...)](nanobind::module_& m) mutable {
        std::apply([&](auto&&... unpacked) { m.def(name, Func, unpacked...); }, args);
      });
}

template <auto Func, typename... Args>
inline AutoRegister package_func(const char* package, const char* name, Args&&... args) {
  return AutoRegister(std::string(package), 0,
      [name, args = std::make_tuple(std::forward<Args>(args)...)](nanobind::module_& m) mutable {
        std::apply([&](auto&&... unpacked) { m.def(name, Func, unpacked...); }, args);
      });
}

template <auto Func, typename... Args>
inline AutoRegister package_async(const char* package, const char* name, Args&&... args) {
  return AutoRegister(std::string(package), 0,
      [name, args = std::make_tuple(std::forward<Args>(args)...)](nanobind::module_& m) mutable {
        std::apply([&](auto&&... unpacked) {
          m.def(name, &::easybind::asyncio::AsyncReturnWrapper<Func>::call, unpacked...);
        }, args);
      });
}

template <typename T>
inline AutoRegister global_attr(const char* name, T&& value) {
  return AutoRegister([name, value = std::forward<T>(value)](nanobind::module_& m) mutable {
    m.attr(name) = std::move(value);
  });
}

template <typename T>
inline AutoRegister package_attr(const char* package, const char* name, T&& value) {
  return AutoRegister(std::string(package), 0,
      [name, value = std::forward<T>(value)](nanobind::module_& m) mutable {
        m.attr(name) = std::move(value);
      });
}

template <typename Enum>
inline AutoRegister package_enum(const char* package, const char* name) {
  return AutoRegister(std::string(package), -10, [name](nanobind::module_& m) {
    ::easybind::enums::bind<Enum>(m, name);
  });
}

template <typename Class>
inline AutoRegister package_class(const char* package, const char* name) {
  return AutoRegister(std::string(package), 0, [name](nanobind::module_& m) {
    ::easybind::describe::bind_struct<Class>(m, name);
  });
}

template <typename Class, typename Fn>
inline AutoRegister package_class_methods(const char* package, const char* name, Fn&& fn) {
  return AutoRegister(std::string(package), 0,
      [name, fn = std::forward<Fn>(fn)](nanobind::module_& m) mutable {
        ::easybind::describe::bind_struct<Class>(m, name, fn);
      });
}

template <typename Exc>
inline AutoRegister package_exception(const char* package, const char* name) {
  return AutoRegister(std::string(package), -20, [name](nanobind::module_& m) {
    ::easybind::exceptions::bind<Exc>(m, name);
  });
}

inline AutoRegister package_exception_alias(const char* package,
    const char* module_name, const char* py_name) {
  return AutoRegister(std::string(package), -20, [module_name, py_name](nanobind::module_& m) {
    ::easybind::exceptions::export_alias(m, module_name, py_name);
  });
}

}  // namespace easybind::bind

// Usage: EASYBIND_BIND(expr);
#define EASYBIND_BIND(EXPR) \
  static ::easybind::AutoRegister EASYBIND_DETAIL_CONCAT(_easybind_reg_, __COUNTER__) = (EXPR)

// Usage: EASYBIND_REGISTER([](nanobind::module_& m) { ... });
#define EASYBIND_REGISTER(LAMBDA) \
  EASYBIND_BIND(::easybind::bind::global(LAMBDA))

// Usage: EASYBIND_REGISTER_GLOBAL_NAMED("name", func, nb::arg("x") = 1, ...);
#define EASYBIND_REGISTER_GLOBAL_NAMED(NAME, FUNC, ...)                                            \
  EASYBIND_BIND(::easybind::bind::global_func<FUNC>(NAME __VA_OPT__(, ) __VA_ARGS__))

// Usage: EASYBIND_REGISTER_ATTR("name", value);
#define EASYBIND_REGISTER_ATTR(NAME, VALUE)                                                       \
  EASYBIND_BIND(::easybind::bind::global_attr(NAME, VALUE))

// Usage: EASYBIND_REGISTER_PACKAGE_ATTR("name", value);
#define EASYBIND_REGISTER_PACKAGE_ATTR(NAME, VALUE)                                               \
  EASYBIND_BIND(::easybind::bind::package_attr(k_package, NAME, VALUE))

// Usage: EASYBIND_REGISTER_PACKAGE("pkg.name", [](nanobind::module_& m) { ... });
#define EASYBIND_REGISTER_PACKAGE(PACKAGE, LAMBDA) \
  EASYBIND_BIND(::easybind::bind::package(PACKAGE, 0, LAMBDA))

// Usage: EASYBIND_REGISTER_PACKAGE_PRI("pkg.name", 10, [](nanobind::module_& m) { ... });
#define EASYBIND_REGISTER_PACKAGE_PRI(PACKAGE, PRIORITY, LAMBDA) \
  EASYBIND_BIND(::easybind::bind::package(PACKAGE, PRIORITY, LAMBDA))

// Usage: EASYBIND_REGISTER_ENUM(EnumType);
#define EASYBIND_REGISTER_ENUM(TYPE)                                                              \
  EASYBIND_BIND(::easybind::bind::package_enum<TYPE>(k_package, #TYPE))

// Usage: EASYBIND_REGISTER_FUNC(func, nb::arg("x") = 1, ...);
#define EASYBIND_REGISTER_FUNC(FUNC, ...)                                                         \
  EASYBIND_BIND(::easybind::bind::package_func<FUNC>(k_package, #FUNC __VA_OPT__(, ) __VA_ARGS__))

// Usage: EASYBIND_REGISTER_ASYNC(func, nb::arg("x") = 1, ...);
#define EASYBIND_REGISTER_ASYNC(FUNC, ...)                                                        \
  EASYBIND_BIND(::easybind::bind::package_async<FUNC>(k_package, #FUNC __VA_OPT__(, ) __VA_ARGS__))

// Usage: EASYBIND_REGISTER_CLASS(Type)
#define EASYBIND_REGISTER_CLASS(TYPE)                                                             \
  EASYBIND_BIND(::easybind::bind::package_class<TYPE>(k_package, #TYPE))

// Usage: EASYBIND_REGISTER_CLASS_METHODS(Type, EASYBIND_DEF_METHOD(...), ...);
#define EASYBIND_REGISTER_CLASS_METHODS(TYPE, ...)                                                 \
  EASYBIND_BIND(::easybind::bind::package_class_methods<TYPE>(k_package, #TYPE, [](auto& cls) {   \
    __VA_ARGS__                                                                                   \
  }))

// Usage: EASYBIND_REGISTER_EXCEPTION(ExceptionType);
#define EASYBIND_REGISTER_EXCEPTION(TYPE)                                                         \
  EASYBIND_BIND(::easybind::bind::package_exception<TYPE>(k_package, #TYPE))

// Usage: EASYBIND_IMPORT_EXCEPTION("pkg.module", "PyName");
#define EASYBIND_IMPORT_EXCEPTION(MODULE_NAME, PY_NAME)                                            \
  EASYBIND_BIND(::easybind::bind::package_exception_alias(k_package, MODULE_NAME, PY_NAME))


// -----------------------------
// Disabled bindings (no-ops)
// -----------------------------
#else

#define EASYBIND_BIND(...)
#define EASYBIND_REGISTER(...)
#define EASYBIND_REGISTER_PACKAGE(...)
#define EASYBIND_REGISTER_PACKAGE_PRI(...)
#define EASYBIND_REGISTER_ENUM(...)
#define EASYBIND_REGISTER_FUNC(...)
#define EASYBIND_REGISTER_ASYNC(...)
#define EASYBIND_REGISTER_CLASS(...)
#define EASYBIND_REGISTER_CLASS_METHODS(...)
#define EASYBIND_REGISTER_EXCEPTION(...)
#define EASYBIND_IMPORT_EXCEPTION(...)

#endif
