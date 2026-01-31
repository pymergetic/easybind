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
// EASYBIND_NS_MODULE(ns::path, m, is_package, {
//   // bind code...
// });
//
// EASYBIND_NS_MODULE_SHARED_OBJECT(ns::path, module_name, m, is_package, {
//   // bind code...
// });
#define EASYBIND_NS_MODULE(NS, MODULE_VAR, IS_PACKAGE, BODY)                    \
  namespace NS {                                                               \
  namespace {                                                                  \
  void __init_bind__(nanobind::module_& MODULE_VAR) BODY                        \
  const std::string __init_full_name__ =                                        \
      ::pymergetic::easybind::module::detail::namespace_to_full_name(#NS);      \
  ::pymergetic::easybind::module::ModuleNode* __init__ =                        \
      ::pymergetic::easybind::module::ModuleNode::create(                       \
          __init_full_name__,                                                   \
          __init_bind__,                                                       \
          IS_PACKAGE,                                                          \
          false);                                                              \
  }                                                                            \
  }

#define EASYBIND_NS_MODULE_SHARED_OBJECT(NS, MODULE, MODULE_VAR, IS_PACKAGE, BODY) \
  namespace NS {                                                               \
  namespace {                                                                  \
  void __init_bind__(nanobind::module_& MODULE_VAR) BODY                        \
  const std::string __init_full_name__ =                                        \
      ::pymergetic::easybind::module::detail::namespace_to_full_name(#NS);      \
  ::pymergetic::easybind::module::ModuleNode* __init__ =                        \
      ::pymergetic::easybind::module::ModuleNode::create(                       \
          __init_full_name__,                                                   \
          __init_bind__,                                                       \
          IS_PACKAGE,                                                          \
          true);                                                               \
  }                                                                            \
  }                                                                            \
  NB_MODULE(MODULE, MODULE_VAR) {                                              \
    const std::string __init_full_name__ =                                      \
        ::pymergetic::easybind::module::detail::namespace_to_full_name(#NS);    \
    ::pymergetic::easybind::module::apply_init(                                 \
        ::pymergetic::easybind::module::ModuleNode::from(__init_full_name__),   \
        MODULE_VAR);                                                           \
  }

#define EASYBIND_NS_MODULE_EXTEND(NS, MODULE_VAR, BODY)                         \
  namespace NS {                                                               \
  namespace {                                                                  \
  void __init_bind__(nanobind::module_& MODULE_VAR) BODY                        \
  const std::string __init_full_name__ =                                        \
      ::pymergetic::easybind::module::detail::namespace_to_full_name(#NS);      \
  ::pymergetic::easybind::module::ModuleNode* __init__ =                        \
      ::pymergetic::easybind::module::ModuleNode::extend(                       \
          __init_full_name__,                                                   \
          __init_bind__);                                                      \
  }                                                                            \
  }
