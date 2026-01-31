# easybind

Simple self-registering helpers for distributed nanobind bindings.

## Python layer
The Python package is implemented as native extensions. It exposes:

- `pymergetic.easybind` (core helpers and macros)
- `pymergetic.easybind.module` (module tree API)
- `pymergetic.easybind.sample` (demo bindings)

## Build-time SDK
`easybind` provides CMake helpers for hybrid extensions:

- `easybind::build_interface` (INTERFACE): baseline C++20 + PIC + shared include paths
- `easybind_add_extension(target ...)`: wraps `nanobind_add_module(... NB_SHARED ...)`

Example:
```cmake
find_package(easybind CONFIG REQUIRED)
easybind_add_extension(my_module src_bind/pymergetic/my_pkg/__cpp__/module.cpp)
target_link_libraries(my_module PRIVATE easybind::easybind)
```

## Core idea
- Each namespace/module defines a `ModuleNode` and a bind callback.
- The module entry point calls `apply_init` to run the callback and recurse.
- Submodules are created on demand and registered in `sys.modules`.
- Shared-object modules are marked so recursion stops at their boundary.
- A minimal sample module lives at `pymergetic.easybind.sample`.

## Developer note: layout rules
- `__init__.cpp` marks the Python boundary (NB_MODULE) for a package/module.
- `node.cpp/.hpp` is the pure C++ module-tree core.
- `ns_module.hpp` defines the `EASYBIND_NS_MODULE*` macros.
- Directory layout mirrors namespaces and Python modules.

## Smallest possible example
### 1) Define a C++ type (normal code)
```cpp
#pragma once

#include <string>

struct PeerInfo {
    std::string peer_id;
    int transport = 0;
};
```

### 2) Bind it in a separate file (module node)
```cpp
#include <pymergetic/easybind/bind.hpp>

struct PeerInfo;  // forward declare or include the header

EASYBIND_NS_MODULE(pymergetic::my_pkg, m, false, {
    nanobind::class_<PeerInfo>(m, "PeerInfo")
        .def(nanobind::init<>())
        .def_rw("peer_id", &PeerInfo::peer_id)
        .def_rw("transport", &PeerInfo::transport);
});
```

### 3) Module entry point (shared-object boundary)
Use this only for the package that has its own `.so` and NB_MODULE entry point.
Do not pair it with `EASYBIND_NS_MODULE` for the same `pymergetic::my_pkg` name.
If you need to add bindings from another file, use `EASYBIND_NS_MODULE_EXTEND`
to extend the same module node instead.
```cpp
#include <pymergetic/easybind/bind.hpp>

EASYBIND_NS_MODULE_SHARED_OBJECT(pymergetic::my_pkg, my_pkg, m, true, {
    m.doc() = "pymergetic.my_pkg module";
});
```

### 4) Extend from another file
```cpp
#include <pymergetic/easybind/bind.hpp>

EASYBIND_NS_MODULE_EXTEND(pymergetic::my_pkg, m, {
    m.def("ping", [] { return "pong"; });
});
```


