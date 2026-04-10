# Helpers for CMake projects that depend on the pip-installed easybind package.
# Bootstrap (once per project):
#   find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
#   execute_process(COMMAND "${Python_EXECUTABLE}" -c
#     "import pathlib, easybind; print(pathlib.Path(easybind.__file__).resolve().parent / 'cmake' / 'easybind_pip.cmake')"
#     OUTPUT_VARIABLE _eb_pip OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ERROR_IS_FATAL ANY)
#   include("${_eb_pip}")
#   easybind_pip_setup()

# Must run before include_guard: guard skips the whole file on re-include, and this must be this file’s directory.
get_filename_component(_EASYBIND_PIP_CMAKE_DIR "${CMAKE_CURRENT_LIST_FILE}" DIRECTORY)
include_guard(GLOBAL)

function(easybind_pip_setup)
  if(NOT Python_FOUND)
    find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)
  endif()

  execute_process(
    COMMAND "${Python_EXECUTABLE}" -m nanobind --cmake_dir
    OUTPUT_STRIP_TRAILING_WHITESPACE
    OUTPUT_VARIABLE nanobind_ROOT
    COMMAND_ERROR_IS_FATAL ANY
  )
  find_package(nanobind CONFIG REQUIRED)

  # This file lives in …/site-packages/easybind/cmake/
  get_filename_component(EASYBIND_PKG_DIR "${_EASYBIND_PIP_CMAKE_DIR}/.." ABSOLUTE)
  get_filename_component(EASYBIND_INCLUDE_ROOT "${EASYBIND_PKG_DIR}/.." ABSOLUTE)

  # Prefer explicit path — find_library can fail with NO_DEFAULT_PATH on some setups.
  if(APPLE)
    set(_eb_core "${EASYBIND_PKG_DIR}/libeasybind.dylib")
  elseif(UNIX)
    set(_eb_core "${EASYBIND_PKG_DIR}/libeasybind.so")
  else()
    find_library(
      _eb_core
      NAMES easybind
      PATHS "${EASYBIND_PKG_DIR}"
      NO_DEFAULT_PATH
      REQUIRED
    )
  endif()
  if(NOT EXISTS "${_eb_core}")
    message(FATAL_ERROR "easybind core library not found at ${_eb_core} (EASYBIND_PKG_DIR=${EASYBIND_PKG_DIR})")
  endif()
  set(EASYBIND_CORE_LIB "${_eb_core}")

  include("${_EASYBIND_PIP_CMAKE_DIR}/easybind_dependencies.cmake")

  set(EASYBIND_PKG_DIR "${EASYBIND_PKG_DIR}" PARENT_SCOPE)
  set(EASYBIND_INCLUDE_ROOT "${EASYBIND_INCLUDE_ROOT}" PARENT_SCOPE)
  set(EASYBIND_CORE_LIB "${EASYBIND_CORE_LIB}" PARENT_SCOPE)
endfunction()

function(easybind_pip_link_magic_enum target)
  if(TARGET magic_enum::magic_enum)
    target_link_libraries(${target} PRIVATE magic_enum::magic_enum)
  else()
    target_include_directories(${target} PRIVATE "${magic_enum_SOURCE_DIR}/include")
  endif()
endfunction()

# RPATH so the extension finds libeasybind (and siblings) next to the pip easybind package.
# Pass the same EASYBIND_PKG_DIR you got from easybind_pip_setup().
function(easybind_pip_set_rpath_next_to_easybind target easybind_pkg_dir)
  if(APPLE)
    set(_install "@loader_path/../easybind")
    set(_build "@loader_path;@loader_path/../easybind")
  else()
    set(_install "$ORIGIN/../easybind")
    set(_build "$ORIGIN;${easybind_pkg_dir}")
  endif()
  set_target_properties(${target} PROPERTIES
    INSTALL_RPATH "${_install}"
    BUILD_RPATH "${_build}"
  )
endfunction()
