# easybind

Simple self-registering helpers for distributed nanobind bindings.

## Python layer
The Python package is intentionally thin. It currently exposes only
`__version__` and exists to ship the CMake/headers as a standard distribution.

## Build-time SDK
`easybind` provides the canonical CMake helper for hybrid extensions:

- `easybind::build_interface` (INTERFACE): baseline C++20 + PIC + shared include paths
- `easybind_add_extension(target ...)`: wraps `nanobind_add_module(... NB_SHARED ...)`

Example:
```cmake
find_package(easybind CONFIG REQUIRED)
easybind_add_extension(my_module src_bind/pymergetic/my_pkg/__cpp__/module.cpp)
target_link_libraries(my_module PRIVATE easybind::easybind)
```

## Core idea
- Each binding file registers a callback during static initialization.
- The module entry point calls `easybind::Registry::get().apply_all(m)`.
- No centralized `bind_*` list is required.
- The registry lives in the `easybind` shared library so multiple extensions share it.
- The pip-built package ships `__cpp__` so imports trigger binding registry.
- A minimal sample module lives at `pymergetic.easybind.sample`.

## Developer note: layout rules
- `__init__.cpp` marks the Python boundary (NB_MODULE) for a package/module.
- `__init__.hpp` declares binding-side globals (`__init__`, `__init_bind__`).
- `__module__` (or `node.cpp/.hpp`, `hooks.cpp/.hpp`) is pure C++ core.
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

### 2) Bind it in a separate file (self-register)
```cpp
#include <nanobind/nanobind.h>
#include <pymergetic/easybind/bind_utils.hpp>

namespace nb = nanobind;

struct PeerInfo;  // forward declare or include the header

EASYBIND_REGISTER([](nanobind::module_& m) {
    nb::class_<PeerInfo>(m, "PeerInfo")
        .def(nb::init<>())
        .def_rw("peer_id", &PeerInfo::peer_id)
        .def_rw("transport", &PeerInfo::transport);
});
```

### 3) Module entry point (runs all registered binds)
```cpp
#include <nanobind/nanobind.h>
#include <pymergetic/easybind/registry.hpp>

NB_MODULE(__cpp__, m) {
    easybind::Registry::get().apply_all(m);
}
```

## Exception bindings (cross-package friendly)
Use these helpers so C++ exceptions get stable Python counterparts and can be
re-exported across modules.

Define/bind in the owning module:
```cpp
#include <pymergetic/easybind/bind_utils.hpp>

class SampleError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

EASYBIND_REGISTER_EXCEPTION(SampleError);
```

Re-export from another module:
```cpp
#include <pymergetic/easybind/bind_utils.hpp>

EASYBIND_IMPORT_EXCEPTION("pymergetic.easybind.sample.__cpp__", "SampleError");
```

## Linker note (important)
If you compile your distributed bind files into a static library, the linker may
discard them unless you force a whole-archive link. Example (Linux/GCC/Clang):
```cmake
add_library(my_bindings STATIC ${BIND_SOURCES})
target_link_libraries(__cpp__ PRIVATE
    "-Wl,--whole-archive" my_bindings "-Wl,--no-whole-archive"
)
```


