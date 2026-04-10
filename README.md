# easybind

Simple self-registering helpers for distributed nanobind bindings.

## Install

```bash
pip install easybind
```

```python
import easybind
from easybind import sample  # optional demo module
```

**PyPI project:** [`easybind`](https://pypi.org/project/easybind/) (same name as the **`import`**).

**Version:** Set by **Git tags** at build time (`v0.1.0`, …) via setuptools-scm; see [RELEASING.md](RELEASING.md). At runtime, `easybind._version.__version__` is written when the wheel is built (or use `importlib.metadata.version("easybind")`).

### Local dev / clangd

An **editable install** configures CMake under `./build/` and generates **`build/compile_commands.json`**, which **clangd** picks up via [`.clangd`](.clangd) — same database as the Python build, no second configure step:

```bash
uv pip install -e .    # or: pip install -e .
```

If you need `compile_commands.json` without pip, run [`scripts/clangd-update.sh`](scripts/clangd-update.sh) (plain `cmake -S . -B build …`).

## Python layer
The Python package is implemented as native extensions. It exposes:

- `easybind` (core helpers and macros)
- `easybind.module` (module tree API)
- `easybind.sample` (demo bindings)

## Build-time SDK
Installed wheels ship CMake helpers under **`easybind/cmake/`**:

- **`easybind_pip.cmake`** — `easybind_pip_setup()` finds Python, **`nanobind`** (pip), **`libeasybind`**, and include roots for `#include <easybind/...>`, then pulls in **`easybind_dependencies.cmake`**. Helpers: `easybind_pip_link_magic_enum(target)`, `easybind_pip_set_rpath_next_to_easybind(target easybind_pkg_dir)`.
- **`easybind_dependencies.cmake`** — pins **nanobind**, **magic_enum**, **reflect-cpp** (same tags as this repo). Use **`easybind_fetch_third_party_deps()`** to pull all three, or **`easybind_fetch_nanobind()`** / **`easybind_fetch_magic_enum()`** / **`easybind_fetch_reflect_cpp()`** when you only need a subset (e.g. pip already provides nanobind, so call only **`easybind_fetch_magic_enum()`**).

Typical consumer bootstrap:

```cmake
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
execute_process(COMMAND "${Python_EXECUTABLE}" -c
  "import pathlib, easybind; print(pathlib.Path(easybind.__file__).resolve().parent / 'cmake' / 'easybind_pip.cmake')"
  OUTPUT_VARIABLE _eb_pip OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY)
include("${_eb_pip}")
easybind_pip_setup()
easybind_fetch_magic_enum()   # if you include easybind headers that need magic_enum
```

When developing **inside** this repository, `easybind_add_extension(...)` is defined in the top-level `CMakeLists.txt` (not shipped in the wheel).

## Bumping `{distribution}~=…` pins (downstream projects)

Use **`easybind.devtools`** or the **`easybind-pin-pyproject`** CLI to rewrite every compatible-release line **`{name}~=X.Y.Z`** in a **`pyproject.toml`** for any PyPI **distribution** name **`name`** (not only **easybind**). Examples:

- **cppdantic** pinning **easybind** in several tables.
- A future project pinning **cppdantic** the same way: pass **`--distribution cppdantic`**.

**CLI** (defaults: **`distribution=easybind`**, version = latest on PyPI; run from the tree that contains **`pyproject.toml`**):

```bash
easybind-pin-pyproject --dry-run
easybind-pin-pyproject
easybind-pin-pyproject --from-github pymergetic/easybind   # latest v* tag via GitHub API (PyPI may lag)
easybind-pin-pyproject --installed
easybind-pin-pyproject --version 0.2.3
easybind-pin-pyproject --distribution cppdantic
easybind-pin-pyproject --pyproject /path/to/pyproject.toml
```

Use **`GITHUB_TOKEN`** for private GitHub repos or higher API rate limits.

**Other devtools CLIs** (after **`pip install`** / **`uv pip install -e .`**, or run **`scripts/…`** shims from a git checkout):

```bash
easybind-release-tag --dry-run    # next v* tag + git push (easybind repo)
easybind-wait-pypi                # poll PyPI until pins resolve (downstream CI)
```

**Library:**

```python
from easybind.devtools import (
    bump_compatible_pins_in_file,
    fetch_pypi_version,
)

# Pin all easybind~= lines to latest PyPI easybind
ver = fetch_pypi_version("easybind")
bump_compatible_pins_in_file("pyproject.toml", "easybind", ver)

# Pin all cppdantic~= lines to latest PyPI cppdantic (e.g. downstream of cppdantic)
ver = fetch_pypi_version("cppdantic")
bump_compatible_pins_in_file("pyproject.toml", "cppdantic", ver)
```

Shorthands **`bump_easybind_compatible_pins`** / **`bump_easybind_compatible_pins_in_file`** remain for **`distribution=\"easybind\"`** only.

**CI (downstream):** **`easybind-wait-pypi`** (or **`scripts/wait_pypi_release.py`**) wraps **`wait_pypi_for_compatible_pin`** in **`easybind.devtools`** — poll PyPI until the pinned version exists. From a checkout without install, set **`PYTHONPATH`** to this repo’s **`src`** or use the shim script.

## Core idea
- Each namespace/module defines a `ModuleNode` and a bind callback.
- The module entry point calls `apply_init` to run the callback and recurse.
- Submodules are created on demand and registered in `sys.modules`.
- Shared-object modules are marked so recursion stops at their boundary.
- A minimal sample module lives at `easybind.sample`.

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
#include <easybind/bind.hpp>

struct PeerInfo;  // forward declare or include the header

EASYBIND_NS_MODULE(my_pkg, m, false, {
    nanobind::class_<PeerInfo>(m, "PeerInfo")
        .def(nanobind::init<>())
        .def_rw("peer_id", &PeerInfo::peer_id)
        .def_rw("transport", &PeerInfo::transport);
});
```

### 3) Module entry point (shared-object boundary)
Use this only for the package that has its own `.so` and NB_MODULE entry point.
Do not pair it with `EASYBIND_NS_MODULE` for the same `my_pkg` name.
If you need to add bindings from another file, use `EASYBIND_NS_MODULE_EXTEND`
to extend the same module node instead.
```cpp
#include <easybind/bind.hpp>

EASYBIND_NS_MODULE_SHARED_OBJECT(my_pkg, my_pkg, m, true, {
    m.doc() = "my_pkg module";
});
```

### 4) Extend from another file
```cpp
#include <easybind/bind.hpp>

EASYBIND_NS_MODULE_EXTEND(my_pkg, m, {
    m.def("ping", [] { return "pong"; });
});
```

