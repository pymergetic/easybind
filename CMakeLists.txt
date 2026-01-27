cmake_minimum_required(VERSION 3.25)

project(easybind LANGUAGES CXX)

include(GNUInstallDirs)
include(FetchContent)

option(EASYBIND_SKIP_INSTALL "Skip installing easybind artifacts" OFF)
if(NOT PROJECT_IS_TOP_LEVEL)
    set(EASYBIND_SKIP_INSTALL ON)
endif()

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

# Nanobind - fetch so builds don't depend on a preinstalled CMake package.
FetchContent_Declare(
    nanobind
    GIT_REPOSITORY https://github.com/wjakob/nanobind.git
    GIT_TAG v2.9.2
)
FetchContent_MakeAvailable(nanobind)

FetchContent_Declare(
    magic_enum
    GIT_REPOSITORY https://github.com/Neargye/magic_enum.git
    GIT_TAG v0.9.6
)
FetchContent_MakeAvailable(magic_enum)

# Some nanobind configurations don't automatically attach Python include dirs.
foreach(_nb_tgt IN ITEMS nanobind nanobind-static nanobind_shared)
    if(TARGET ${_nb_tgt})
        target_link_libraries(${_nb_tgt} PRIVATE ${EASYBIND_PYTHON_TARGET})
    endif()
endforeach()

function(easybind_add_extension target_name)
    nanobind_add_module(${target_name} NB_SHARED ${ARGN})
    target_link_libraries(${target_name} PRIVATE ${EASYBIND_PYTHON_TARGET})
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

easybind_add_extension(easybind_sample_internal
    src_cpp/pymergetic/easybind/sample/__init__.cpp
    src_cpp/pymergetic/easybind/sample/__cpp__.cpp
    src_cpp/pymergetic/easybind/module/__init__.cpp
)
set_target_properties(easybind_sample_internal PROPERTIES
    OUTPUT_NAME "sample"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymergetic/easybind"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymergetic/easybind"
)
target_include_directories(easybind_sample_internal PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src_cpp
)
if(TARGET magic_enum::magic_enum)
    target_link_libraries(easybind_sample_internal PRIVATE magic_enum::magic_enum)
    target_include_directories(easybind_sample_internal PRIVATE
        ${magic_enum_SOURCE_DIR}/include
    )
else()
    target_include_directories(easybind_sample_internal PRIVATE
        ${magic_enum_SOURCE_DIR}/include
    )
endif()
if(APPLE)
    set_target_properties(easybind_sample_internal PROPERTIES
        BUILD_RPATH "@loader_path"
        INSTALL_RPATH "@loader_path"
    )
else()
    set_target_properties(easybind_sample_internal PROPERTIES
        BUILD_RPATH "$ORIGIN"
        INSTALL_RPATH "$ORIGIN"
    )
endif()

if(NOT EASYBIND_SKIP_INSTALL)
    install(TARGETS easybind_sample_internal
        LIBRARY DESTINATION pymergetic/easybind
        RUNTIME DESTINATION pymergetic/easybind
        ARCHIVE DESTINATION pymergetic/easybind
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
endif()

easybind_add_extension(easybind_internal
    src_cpp/pymergetic/easybind/__cpp__.cpp
    src_cpp/pymergetic/easybind/__init__.cpp
    src_cpp/pymergetic/easybind/module/__init__.cpp
)
set_target_properties(easybind_internal PROPERTIES
    OUTPUT_NAME "__init__"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymergetic/easybind"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymergetic/easybind"
)
target_include_directories(easybind_internal PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src_cpp
)
if(TARGET magic_enum::magic_enum)
    target_link_libraries(easybind_internal PRIVATE magic_enum::magic_enum)
    target_include_directories(easybind_internal PRIVATE
        ${magic_enum_SOURCE_DIR}/include
    )
else()
    target_include_directories(easybind_internal PRIVATE
        ${magic_enum_SOURCE_DIR}/include
    )
endif()
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

if(NOT EASYBIND_SKIP_INSTALL)
    install(TARGETS easybind_internal
        LIBRARY DESTINATION pymergetic/easybind
        RUNTIME DESTINATION pymergetic/easybind
        ARCHIVE DESTINATION pymergetic/easybind
    )

    install(DIRECTORY src_py/pymergetic
        DESTINATION .
    )
endif()
