#include <pymergetic/easybind/prelude.hpp>
#include <pymergetic/easybind/refresh.hpp>

#include <nanobind/stl/vector.h>

// Bootstrap module for the easybind registry.
EASYBIND_SHARED_OBJECT();

// Ensure the easybind package appears in the package registry.
EASYBIND_REGISTER_PACKAGE(EASYBIND_BOOTSTRAP_PACKAGE, [](nanobind::module_&) {});

// Expose registry inspection for Python.
EASYBIND_REGISTER_GLOBAL_NAMED(
    "registered_modules",
    ::easybind::registered_packages,
    "Return registered module names.");

// Expose registry refresh for Python.
EASYBIND_REGISTER_GLOBAL_NAMED(
    "refresh_bindings",
    ::easybind::refresh_bindings,
    "Apply newly registered bindings across all packages.");
