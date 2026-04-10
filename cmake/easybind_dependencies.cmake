# Single source of truth for FetchContent pins used by easybind and downstream CMake projects.
# Include from the installed package: include("${EASYBIND_PKG_DIR}/cmake/easybind_dependencies.cmake")

include_guard(GLOBAL)

include(FetchContent)

set(EASYBIND_NANOBIND_GIT_TAG "v2.12.0"
    CACHE STRING "nanobind Git tag (align with easybind’s nanobind / PyPI pin)")
set(EASYBIND_MAGIC_ENUM_GIT_TAG "v0.9.7" CACHE STRING "magic_enum Git tag")
set(EASYBIND_REFLECT_CPP_GIT_TAG "v0.24.0" CACHE STRING "reflect-cpp Git tag")

function(easybind_fetch_nanobind)
  FetchContent_GetProperties(nanobind)
  if(nanobind_POPULATED)
    return()
  endif()
  FetchContent_Declare(
    nanobind
    GIT_REPOSITORY https://github.com/wjakob/nanobind.git
    GIT_TAG "${EASYBIND_NANOBIND_GIT_TAG}"
  )
  FetchContent_MakeAvailable(nanobind)
endfunction()

function(easybind_fetch_magic_enum)
  FetchContent_GetProperties(magic_enum)
  if(magic_enum_POPULATED)
    return()
  endif()
  FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG "${EASYBIND_MAGIC_ENUM_GIT_TAG}"
  )
  FetchContent_MakeAvailable(magic_enum)
endfunction()

function(easybind_fetch_reflect_cpp)
  FetchContent_GetProperties(reflect_cpp)
  if(reflect_cpp_POPULATED)
    return()
  endif()
  FetchContent_Declare(
    reflect_cpp
    GIT_REPOSITORY https://github.com/getml/reflect-cpp.git
    GIT_TAG "${EASYBIND_REFLECT_CPP_GIT_TAG}"
  )
  FetchContent_MakeAvailable(reflect_cpp)
endfunction()

function(easybind_fetch_third_party_deps)
  easybind_fetch_nanobind()
  easybind_fetch_magic_enum()
  easybind_fetch_reflect_cpp()
endfunction()
