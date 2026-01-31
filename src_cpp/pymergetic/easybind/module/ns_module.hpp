#pragma once

#include <cstring>
#include <string>

#include <pymergetic/easybind/module/node.hpp>

//
// Helpers to declare easybind module nodes with minimal boilerplate.
//

namespace pymergetic::easybind::module::detail {

inline std::string namespace_to_full_name(const char* ns) {
  std::string out;
  out.reserve(std::strlen(ns));
  for (size_t i = 0; ns[i] != '\0';) {
    if (ns[i] == ':' && ns[i + 1] == ':') {
      out.push_back('.');
      i += 2;
    } else {
      out.push_back(ns[i]);
      ++i;
    }
  }
  return out;
}

}  // namespace pymergetic::easybind::module::detail

// Usage:
// EASYBIND_NS_MODULE(ns::path, {
//   // bind code...
// });
//
// EASYBIND_NS_MODULE_SHARED_OBJECT(ns::path, module_name, {
//   // bind code...
// });
#define EASYBIND_NS_MODULE(NS, IS_PACKAGE, BODY)                               \
  namespace NS {                                                               \
  namespace {                                                                  \
  void __init_bind__(nanobind::module_& m) BODY                                \
  }                                                                            \
  ::pymergetic::easybind::module::ModuleNode* __init__ =                        \
      ::pymergetic::easybind::module::ModuleNode::create(                       \
          ::pymergetic::easybind::module::detail::namespace_to_full_name(#NS),  \
          __init_bind__,                                                       \
          IS_PACKAGE,                                                          \
          false);                                                              \
  }

#define EASYBIND_NS_MODULE_SHARED_OBJECT(NS, MODULE, IS_PACKAGE, BODY)          \
  namespace NS {                                                               \
  namespace {                                                                  \
  void __init_bind__(nanobind::module_& m) BODY                                \
  }                                                                            \
  ::pymergetic::easybind::module::ModuleNode* __init__ =                        \
      ::pymergetic::easybind::module::ModuleNode::create(                       \
          ::pymergetic::easybind::module::detail::namespace_to_full_name(#NS),  \
          __init_bind__,                                                       \
          IS_PACKAGE,                                                          \
          true);                                                               \
  }                                                                            \
  NB_MODULE(MODULE, m) {                                                       \
    ::pymergetic::easybind::module::apply_init(::NS::__init__, m);              \
  }
