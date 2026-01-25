#pragma once

#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>

#include <string>

#include <pymergetic/easybind/asyncio_asio.hpp>
#include <pymergetic/easybind/bind_utils.hpp>

namespace easybind {

inline auto arg(const char* name) {
  return nanobind::arg(name);
}

namespace detail {

inline std::string ns_to_package(const char* ns) {
  std::string out;
  out.reserve(std::char_traits<char>::length(ns));
  for (std::size_t i = 0; ns[i] != '\0';) {
    if (ns[i] == ':' && ns[i + 1] == ':') {
      out.push_back('.');
      i += 2;
      continue;
    }
    out.push_back(ns[i]);
    ++i;
  }
  return out;
}

}  // namespace detail

}  // namespace easybind

// Usage: EASYBIND_MODULE("pkg.name"), EASYBIND_MODULE("pkg.name", module_name),
//        EASYBIND_MODULE("pkg.name", module_name, m)
#define EASYBIND_DETAIL_MODULE1(PACKAGE)                                                          \
  EASYBIND_DETAIL_MODULE_IMPL(PACKAGE, __cpp__, m)
#define EASYBIND_DETAIL_MODULE2(PACKAGE, MODULE_NAME)                                              \
  EASYBIND_DETAIL_MODULE_IMPL(PACKAGE, MODULE_NAME, m)
#define EASYBIND_DETAIL_MODULE3(PACKAGE, MODULE_NAME, MODULE_VAR)                                  \
  EASYBIND_DETAIL_MODULE_IMPL(PACKAGE, MODULE_NAME, MODULE_VAR)
#define EASYBIND_DETAIL_MODULE_CHOOSER(...)                                                        \
  EASYBIND_DETAIL_MODULE_CHOOSER_IMPL(__VA_ARGS__,                                                 \
      EASYBIND_DETAIL_MODULE3, EASYBIND_DETAIL_MODULE2, EASYBIND_DETAIL_MODULE1)
#define EASYBIND_DETAIL_MODULE_CHOOSER_IMPL(_1, _2, _3, NAME, ...) NAME
#define EASYBIND_MODULE(...)                                                                       \
  EASYBIND_DETAIL_MODULE_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
#define EASYBIND_DETAIL_MODULE_IMPL(PACKAGE, MODULE_NAME, MODULE_VAR)                              \
  static const char* k_package = PACKAGE;                                                          \
  NB_MODULE(MODULE_NAME, MODULE_VAR) {                                                            \
    MODULE_VAR.doc() = std::string(PACKAGE) + " module";                                           \
    ::easybind::Registry::get().apply_all_for(PACKAGE, MODULE_VAR);                               \
    MODULE_VAR.def("refresh_bindings", [MODULE_VAR]() {                                           \
      auto module = MODULE_VAR;                                                                   \
      ::easybind::Registry::get().apply_pending_for(PACKAGE, module);                             \
    }, "Apply newly registered bindings for this module.");                                       \
  }

// Usage: EASYBIND_NS(my::namespace) { ... }
#define EASYBIND_NS(NS)                                                                            \
  EASYBIND_NS_IMPL(NS, EASYBIND_DETAIL_CONCAT(_easybind_pkg_, __COUNTER__))
#define EASYBIND_NS_IMPL(NS, PKG_VAR)                                                              \
  static const std::string PKG_VAR = ::easybind::detail::ns_to_package(#NS);                        \
  EASYBIND_MODULE(PKG_VAR.c_str());                                                                \
  namespace NS

// Usage: EASYBIND_MODULE_PACKAGE() or EASYBIND_MODULE_PACKAGE(module_name)
#define EASYBIND_DETAIL_MODULE_PACKAGE0()                                                         \
  EASYBIND_MODULE(k_package, __cpp__, m)
#define EASYBIND_DETAIL_MODULE_PACKAGE1(MODULE_NAME)                                               \
  EASYBIND_MODULE(k_package, MODULE_NAME, m)
#define EASYBIND_DETAIL_MODULE_PACKAGE_CHOOSER(...)                                                \
  EASYBIND_DETAIL_MODULE_PACKAGE_CHOOSER_IMPL(                                                     \
      _easybind_dummy, __VA_ARGS__ __VA_OPT__(,) EASYBIND_DETAIL_MODULE_PACKAGE1,                  \
      EASYBIND_DETAIL_MODULE_PACKAGE0)
#define EASYBIND_DETAIL_MODULE_PACKAGE_CHOOSER_IMPL(_1, _2, NAME, ...) NAME
#define EASYBIND_MODULE_PACKAGE(...)                                                               \
  EASYBIND_DETAIL_MODULE_PACKAGE_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

// Usage: EASYBIND_MODULE_ALL() or EASYBIND_MODULE_ALL(module_name)
#define EASYBIND_DETAIL_MODULE_ALL0()                                                              \
  EASYBIND_DETAIL_MODULE_ALL_IMPL(__cpp__, m)
#define EASYBIND_DETAIL_MODULE_ALL1(MODULE_NAME)                                                   \
  EASYBIND_DETAIL_MODULE_ALL_IMPL(MODULE_NAME, m)
#define EASYBIND_DETAIL_MODULE_ALL_CHOOSER(...)                                                    \
  EASYBIND_DETAIL_MODULE_ALL_CHOOSER_IMPL(                                                         \
      _easybind_dummy, __VA_ARGS__ __VA_OPT__(,) EASYBIND_DETAIL_MODULE_ALL1,                       \
      EASYBIND_DETAIL_MODULE_ALL0)
#define EASYBIND_DETAIL_MODULE_ALL_CHOOSER_IMPL(_1, _2, NAME, ...) NAME
#define EASYBIND_MODULE_ALL(...)                                                                   \
  EASYBIND_DETAIL_MODULE_ALL_CHOOSER(__VA_ARGS__)(__VA_ARGS__)
#define EASYBIND_DETAIL_MODULE_ALL_IMPL(MODULE_NAME, MODULE_VAR)                                   \
  NB_MODULE(MODULE_NAME, MODULE_VAR) {                                                            \
    MODULE_VAR.doc() = "easybind registry bootstrap module";                                       \
    ::easybind::Registry::get().apply_all(MODULE_VAR);                                             \
    MODULE_VAR.def("refresh_bindings", [MODULE_VAR]() {                                           \
      auto module = MODULE_VAR;                                                                   \
      ::easybind::Registry::get().apply_pending(module);                                          \
    }, "Apply newly registered bindings across all packages.");                                   \
  }
