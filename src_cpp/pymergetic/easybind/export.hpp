#pragma once

// Cross-platform export/import helpers for shared libraries.
#if defined(_WIN32)
#  define EASYBIND_EXPORT __declspec(dllexport)
#  define EASYBIND_IMPORT __declspec(dllimport)
#else
#  define EASYBIND_EXPORT __attribute__((visibility("default")))
#  define EASYBIND_IMPORT __attribute__((visibility("default")))
#endif

// Project-wide API macro for the easybind core library.
//
// Child libraries should define their own <LIB>_API macro using the same
// EASYBIND_EXPORT/EASYBIND_IMPORT helpers and a <LIB>_BUILD switch.
// Example:
//   #if defined(MYLIB_BUILD)
//   #  define MYLIB_API EASYBIND_EXPORT
//   #else
//   #  define MYLIB_API EASYBIND_IMPORT
//   #endif
#if defined(EASYBIND_BUILD)
#  define EASYBIND_API EASYBIND_EXPORT
#else
#  define EASYBIND_API EASYBIND_IMPORT
#endif
