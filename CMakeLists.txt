cmake_minimum_required(VERSION 3.25)

project(easybind LANGUAGES CXX)

include(GNUInstallDirs)
include(FetchContent)

# Ensure Python development headers/targets are available for nanobind + modules.
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)

# Prefer Python::Module (extension modules), fall back to Python::Python.
set(EASYBIND_PYTHON_TARGET "")
if(TARGET Python::Module)
    set(EASYBIND_PYTHON_TARGET Python::Module)
elseif(TARGET Python::Python)
    set(EASYBIND_PYTHON_TARGET Python::Python)
else()
    message(FATAL_ERROR "Python imported target not found (expected Python::Module or Python::Python)")
endif()

# Nanobind (tooling) - fetch so builds don't depend on a preinstalled CMake package.
FetchContent_Declare(
    nanobind
    GIT_REPOSITORY https://github.com/wjakob/nanobind.git
    GIT_TAG v2.9.2
)
FetchContent_MakeAvailable(nanobind)

# Boost headers (Boost.Asio) for sample async demo.
find_path(PYMERGETIC_BOOST_INCLUDE_DIR boost/asio.hpp)
if(NOT PYMERGETIC_BOOST_INCLUDE_DIR)
    set(_pymergetic_boost_ver "1.85.0")
    string(REPLACE "." "_" _pymergetic_boost_ver_us "${_pymergetic_boost_ver}")
    set(_pymergetic_boost_url "https://archives.boost.io/release/${_pymergetic_boost_ver}/source/boost_${_pymergetic_boost_ver_us}.tar.gz")
    FetchContent_Declare(pymergetic_boost URL ${_pymergetic_boost_url})
    FetchContent_MakeAvailable(pymergetic_boost)
    set(PYMERGETIC_BOOST_INCLUDE_DIR "${pymergetic_boost_SOURCE_DIR}")
endif()

# magic_enum (required for enum auto-binding).
FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG v0.9.7
)
FetchContent_MakeAvailable(magic_enum)

# Some nanobind configurations don't automatically attach Python include dirs.
foreach(_nb_tgt IN ITEMS nanobind nanobind-static nanobind_shared)
    if(TARGET ${_nb_tgt})
        target_link_libraries(${_nb_tgt} PRIVATE ${EASYBIND_PYTHON_TARGET})
    endif()
endforeach()

add_library(easybind SHARED
    src_cpp/pymergetic/easybind/registry.cpp
    src_cpp/pymergetic/easybind/sample/sample.cpp
)
add_library(easybind::easybind ALIAS easybind)

target_compile_features(easybind PUBLIC cxx_std_20)

target_include_directories(easybind PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src_cpp>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_include_directories(easybind PUBLIC
    $<BUILD_INTERFACE:${magic_enum_SOURCE_DIR}/include>
)

nanobind_add_module(easybind_internal NB_SHARED
    src_bind/pymergetic/easybind/module.cpp
)
target_link_libraries(easybind_internal PRIVATE easybind::easybind)
target_link_libraries(easybind_internal PRIVATE ${EASYBIND_PYTHON_TARGET})
target_include_directories(easybind_internal PRIVATE ${magic_enum_SOURCE_DIR}/include)
set_target_properties(easybind_internal PROPERTIES OUTPUT_NAME "_internal")
if(APPLE)
    set_target_properties(easybind_internal PROPERTIES
        BUILD_RPATH "@loader_path"
        INSTALL_RPATH "@loader_path"
    )
else()
    set_target_properties(easybind_internal PROPERTIES
        BUILD_RPATH "$ORIGIN"
        INSTALL_RPATH "$ORIGIN"
    )
endif()

nanobind_add_module(easybind_sample_internal NB_SHARED
    src_bind/pymergetic/easybind/sample/module.cpp
    src_bind/pymergetic/easybind/sample/bind.cpp
)
target_link_libraries(easybind_sample_internal PRIVATE easybind::easybind)
target_link_libraries(easybind_sample_internal PRIVATE ${EASYBIND_PYTHON_TARGET})
target_include_directories(easybind_sample_internal PRIVATE ${magic_enum_SOURCE_DIR}/include)
target_include_directories(easybind_sample_internal PRIVATE ${PYMERGETIC_BOOST_INCLUDE_DIR})
target_compile_definitions(easybind_sample_internal PRIVATE BOOST_ERROR_CODE_HEADER_ONLY=1 BOOST_SYSTEM_NO_DEPRECATED=1)
set_target_properties(easybind_sample_internal PROPERTIES OUTPUT_NAME "_sample")
if(APPLE)
    set_target_properties(easybind_sample_internal PROPERTIES
        BUILD_RPATH "@loader_path;@loader_path/.."
        INSTALL_RPATH "@loader_path;@loader_path/.."
    )
else()
    set_target_properties(easybind_sample_internal PROPERTIES
        BUILD_RPATH "$ORIGIN;$ORIGIN/.."
        INSTALL_RPATH "$ORIGIN;$ORIGIN/.."
    )
endif()

install(DIRECTORY src_cpp/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
install(DIRECTORY ${magic_enum_SOURCE_DIR}/include/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS easybind
    EXPORT easybindTargets
    LIBRARY DESTINATION pymergetic/easybind
    RUNTIME DESTINATION pymergetic/easybind
    ARCHIVE DESTINATION pymergetic/easybind
)

install(TARGETS easybind_internal
    LIBRARY DESTINATION pymergetic/easybind
    RUNTIME DESTINATION pymergetic/easybind
    ARCHIVE DESTINATION pymergetic/easybind
)

install(TARGETS easybind_sample_internal
    LIBRARY DESTINATION pymergetic/easybind/sample
    RUNTIME DESTINATION pymergetic/easybind/sample
    ARCHIVE DESTINATION pymergetic/easybind/sample
)

# If nanobind is built as a shared library, ship it alongside extensions so
# `libnanobind.so` is resolvable at runtime.
if(TARGET nanobind)
    install(TARGETS nanobind
        LIBRARY DESTINATION pymergetic/easybind
        RUNTIME DESTINATION pymergetic/easybind
        ARCHIVE DESTINATION pymergetic/easybind
    )
endif()

install(EXPORT easybindTargets
    FILE easybindTargets.cmake
    NAMESPACE easybind::
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/easybind
)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${CMAKE_CURRENT_LIST_DIR}/cmake/easybindConfig.cmake.in"
    "${CMAKE_CURRENT_BINARY_DIR}/easybindConfig.cmake"
    INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/easybind
)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/easybindConfig.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/easybind
)
