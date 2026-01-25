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

add_library(easybind_build_interface INTERFACE)
add_library(easybind::build_interface ALIAS easybind_build_interface)

target_compile_features(easybind_build_interface INTERFACE cxx_std_20)
set_target_properties(easybind_build_interface PROPERTIES
    INTERFACE_POSITION_INDEPENDENT_CODE ON
)

target_include_directories(easybind_build_interface INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src_cpp>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<BUILD_INTERFACE:${PYMERGETIC_BOOST_INCLUDE_DIR}>
)

function(easybind_add_extension target_name)
    nanobind_add_module(${target_name} NB_SHARED ${ARGN})
    target_link_libraries(${target_name} PRIVATE easybind::build_interface)
    target_link_libraries(${target_name} PRIVATE ${EASYBIND_PYTHON_TARGET})
    target_include_directories(${target_name} PRIVATE ${PYMERGETIC_BOOST_INCLUDE_DIR})
    target_compile_definitions(${target_name} PRIVATE BOOST_ERROR_CODE_HEADER_ONLY=1 BOOST_SYSTEM_NO_DEPRECATED=1)
    if(APPLE)
        set_target_properties(${target_name} PROPERTIES
            BUILD_RPATH "@loader_path"
            INSTALL_RPATH "@loader_path"
        )
    else()
        set_target_properties(${target_name} PROPERTIES
            BUILD_RPATH "$ORIGIN"
            INSTALL_RPATH "$ORIGIN"
        )
    endif()
endfunction()

add_library(easybind SHARED
    src_cpp/pymergetic/easybind/registry.cpp
)
add_library(easybind::easybind ALIAS easybind)

target_compile_features(easybind PUBLIC cxx_std_20)
target_link_libraries(easybind PUBLIC easybind::build_interface)

target_include_directories(easybind PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src_cpp>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
)
target_include_directories(easybind PUBLIC
    $<BUILD_INTERFACE:${magic_enum_SOURCE_DIR}/include>
)
target_include_directories(easybind PRIVATE
    $<BUILD_INTERFACE:${nanobind_SOURCE_DIR}/include>
)

easybind_add_extension(easybind_internal
    src_cpp/pymergetic/easybind/__cpp__.cpp
    src_cpp/pymergetic/easybind/version/__init__.cpp
)
target_link_libraries(easybind_internal PRIVATE easybind::easybind)
target_include_directories(easybind_internal PRIVATE ${magic_enum_SOURCE_DIR}/include)
if(DEFINED ENV{SKBUILD_PROJECT_VERSION})
    set(_easybind_build_version "$ENV{SKBUILD_PROJECT_VERSION}")
else()
    set(_easybind_build_version "0.0.0")
endif()
target_compile_definitions(easybind_internal PRIVATE EASYBIND_BUILD_VERSION="${_easybind_build_version}")
set_target_properties(easybind_internal PROPERTIES
    OUTPUT_NAME "__cpp__"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymergetic/easybind"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymergetic/easybind"
)
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

easybind_add_extension(easybind_version_internal
    src_cpp/pymergetic/easybind/version/__cpp__.cpp
    src_cpp/pymergetic/easybind/version/__init__.cpp
)
target_link_libraries(easybind_version_internal PRIVATE easybind::easybind)
target_include_directories(easybind_version_internal PRIVATE ${magic_enum_SOURCE_DIR}/include)
target_compile_definitions(easybind_version_internal PRIVATE EASYBIND_BUILD_VERSION="${_easybind_build_version}")
set_target_properties(easybind_version_internal PROPERTIES
    OUTPUT_NAME "__cpp__"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymergetic/easybind/version"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymergetic/easybind/version"
)
if(APPLE)
    set_target_properties(easybind_version_internal PROPERTIES
        BUILD_RPATH "@loader_path;@loader_path/.."
        INSTALL_RPATH "@loader_path;@loader_path/.."
    )
else()
    set_target_properties(easybind_version_internal PROPERTIES
        BUILD_RPATH "$ORIGIN;$ORIGIN/.."
        INSTALL_RPATH "$ORIGIN;$ORIGIN/.."
    )
endif()

easybind_add_extension(easybind_sample_internal
    src_cpp/pymergetic/easybind/sample/__cpp__.cpp
    src_cpp/pymergetic/easybind/sample/__init__.cpp
)
target_link_libraries(easybind_sample_internal PRIVATE easybind::easybind)
target_include_directories(easybind_sample_internal PRIVATE ${magic_enum_SOURCE_DIR}/include)
target_compile_definitions(easybind_sample_internal PRIVATE PYMERGETIC_BINDINGS=1)
set_target_properties(easybind_sample_internal PROPERTIES
    OUTPUT_NAME "__cpp__"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymergetic/easybind/sample"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymergetic/easybind/sample"
)
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

install(TARGETS easybind_build_interface
    EXPORT easybindTargets
)

install(TARGETS easybind_internal
    LIBRARY DESTINATION pymergetic/easybind
    RUNTIME DESTINATION pymergetic/easybind
    ARCHIVE DESTINATION pymergetic/easybind
)

install(TARGETS easybind_version_internal
    LIBRARY DESTINATION pymergetic/easybind/version
    RUNTIME DESTINATION pymergetic/easybind/version
    ARCHIVE DESTINATION pymergetic/easybind/version
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
