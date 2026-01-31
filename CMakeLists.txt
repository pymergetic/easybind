cmake_minimum_required(VERSION 3.25)

project(easybind LANGUAGES CXX)

include(GNUInstallDirs)
include(FetchContent)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

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

FetchContent_Declare(
    reflect_cpp
    GIT_REPOSITORY https://github.com/getml/reflect-cpp.git
    GIT_TAG main
)
FetchContent_MakeAvailable(reflect_cpp)

# Some nanobind configurations don't automatically attach Python include dirs.
foreach(_nb_tgt IN ITEMS nanobind nanobind_shared nanobind-static)
    if(TARGET ${_nb_tgt})
        target_link_libraries(${_nb_tgt} PRIVATE ${EASYBIND_PYTHON_TARGET})
        set_target_properties(${_nb_tgt} PROPERTIES
            CXX_VISIBILITY_PRESET default
            VISIBILITY_INLINES_HIDDEN OFF
        )
        target_compile_options(${_nb_tgt} PRIVATE
            $<$<CXX_COMPILER_ID:GNU,Clang>:-fvisibility=default>
        )
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

add_library(pymergetic_easybind__module__ SHARED
    src_cpp/pymergetic/easybind/module/node.cpp
)
set_target_properties(pymergetic_easybind__module__ PROPERTIES
    OUTPUT_NAME "pymergetic-easybind"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymergetic/easybind"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymergetic/easybind"
)
target_include_directories(pymergetic_easybind__module__ PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src_cpp
)
target_compile_definitions(pymergetic_easybind__module__ PRIVATE EASYBIND_BUILD)
if(DEFINED nanobind_SOURCE_DIR)
    target_include_directories(pymergetic_easybind__module__ PRIVATE
        ${nanobind_SOURCE_DIR}/include
    )
endif()
target_link_libraries(pymergetic_easybind__module__ PRIVATE ${EASYBIND_PYTHON_TARGET})
if(TARGET nanobind)
    target_link_libraries(pymergetic_easybind__module__ PRIVATE nanobind)
elseif(TARGET nanobind_shared)
    target_link_libraries(pymergetic_easybind__module__ PRIVATE nanobind_shared)
elseif(TARGET nanobind-static)
    target_link_libraries(pymergetic_easybind__module__ PRIVATE nanobind-static)
    target_compile_definitions(pymergetic_easybind__module__ PRIVATE NB_STATIC)
endif()
if(APPLE)
    set_target_properties(pymergetic_easybind__module__ PROPERTIES
        BUILD_RPATH "@loader_path"
        INSTALL_RPATH "@loader_path"
    )
else()
    set_target_properties(pymergetic_easybind__module__ PROPERTIES
        BUILD_RPATH "$ORIGIN"
        INSTALL_RPATH "$ORIGIN"
    )
endif()
set_target_properties(pymergetic_easybind__module__ PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)
target_compile_options(pymergetic_easybind__module__ PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-fvisibility=hidden>
)

add_library(pymergetic_easybind_sample__module__ SHARED
    src_cpp/pymergetic/easybind/sample/sample.cpp
)
set_target_properties(pymergetic_easybind_sample__module__ PROPERTIES
    OUTPUT_NAME "pymergetic-easybind-sample"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymergetic/easybind/sample"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymergetic/easybind/sample"
)
target_include_directories(pymergetic_easybind_sample__module__ PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src_cpp
)
target_compile_definitions(pymergetic_easybind_sample__module__ PRIVATE EASYBIND_SAMPLE_BUILD)
if(APPLE)
    set_target_properties(pymergetic_easybind_sample__module__ PROPERTIES
        BUILD_RPATH "@loader_path"
        INSTALL_RPATH "@loader_path"
    )
else()
    set_target_properties(pymergetic_easybind_sample__module__ PROPERTIES
        BUILD_RPATH "$ORIGIN"
        INSTALL_RPATH "$ORIGIN"
    )
endif()
set_target_properties(pymergetic_easybind_sample__module__ PROPERTIES
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)
target_compile_options(pymergetic_easybind_sample__module__ PRIVATE
    $<$<CXX_COMPILER_ID:GNU,Clang>:-fvisibility=hidden>
)

easybind_add_extension(pymergetic_easybind_sample__init__
    src_cpp/pymergetic/easybind/sample/__init__.cpp
)
target_link_libraries(pymergetic_easybind_sample__init__ PRIVATE
    pymergetic_easybind__module__
    pymergetic_easybind_sample__module__
)
set_target_properties(pymergetic_easybind_sample__init__ PROPERTIES
    OUTPUT_NAME "__init__"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymergetic/easybind/sample"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymergetic/easybind/sample"
)
target_include_directories(pymergetic_easybind_sample__init__ PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src_cpp
)
if(TARGET magic_enum::magic_enum)
    target_link_libraries(pymergetic_easybind_sample__init__ PRIVATE magic_enum::magic_enum)
    target_include_directories(pymergetic_easybind_sample__init__ PRIVATE
        ${magic_enum_SOURCE_DIR}/include
    )
else()
    target_include_directories(pymergetic_easybind_sample__init__ PRIVATE
        ${magic_enum_SOURCE_DIR}/include
    )
endif()
if(DEFINED reflect_cpp_SOURCE_DIR)
    target_include_directories(pymergetic_easybind_sample__init__ PRIVATE
        ${reflect_cpp_SOURCE_DIR}/include
    )
endif()
if(APPLE)
    set_target_properties(pymergetic_easybind_sample__init__ PROPERTIES
        BUILD_RPATH "@loader_path"
        INSTALL_RPATH "@loader_path"
    )
else()
    set_target_properties(pymergetic_easybind_sample__init__ PROPERTIES
        BUILD_RPATH "$ORIGIN"
        INSTALL_RPATH "$ORIGIN"
    )
endif()

if(NOT EASYBIND_SKIP_INSTALL)
    install(TARGETS pymergetic_easybind_sample__module__
        LIBRARY DESTINATION pymergetic/easybind/sample
        RUNTIME DESTINATION pymergetic/easybind/sample
        ARCHIVE DESTINATION pymergetic/easybind/sample
    )
    install(TARGETS pymergetic_easybind_sample__init__
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
endif()

easybind_add_extension(pymergetic_easybind__init__
    src_cpp/pymergetic/easybind/__init__.cpp
    src_cpp/pymergetic/easybind/module/__init__.cpp
)
target_link_libraries(pymergetic_easybind__init__ PRIVATE pymergetic_easybind__module__)
set_target_properties(pymergetic_easybind__init__ PROPERTIES
    OUTPUT_NAME "__init__"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymergetic/easybind"
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/pymergetic/easybind"
)
target_include_directories(pymergetic_easybind__init__ PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src_cpp
)
if(TARGET magic_enum::magic_enum)
    target_link_libraries(pymergetic_easybind__init__ PRIVATE magic_enum::magic_enum)
    target_include_directories(pymergetic_easybind__init__ PRIVATE
        ${magic_enum_SOURCE_DIR}/include
    )
else()
    target_include_directories(pymergetic_easybind__init__ PRIVATE
        ${magic_enum_SOURCE_DIR}/include
    )
endif()
if(DEFINED reflect_cpp_SOURCE_DIR)
    target_include_directories(pymergetic_easybind__init__ PRIVATE
        ${reflect_cpp_SOURCE_DIR}/include
    )
endif()
if(APPLE)
    set_target_properties(pymergetic_easybind__init__ PROPERTIES
        BUILD_RPATH "@loader_path"
        INSTALL_RPATH "@loader_path"
    )
else()
    set_target_properties(pymergetic_easybind__init__ PROPERTIES
        BUILD_RPATH "$ORIGIN"
        INSTALL_RPATH "$ORIGIN"
    )
endif()

# Build-time stub generation (package layout).
function(easybind_add_stub_package stub_target_prefix module_target)
    foreach(module_name IN LISTS ARGN)
        string(REPLACE "." "/" module_path "${module_name}")
        set(output_path "${CMAKE_CURRENT_SOURCE_DIR}/src_py/${module_path}/__init__.pyi")
        get_filename_component(output_dir "${output_path}" DIRECTORY)
        file(MAKE_DIRECTORY "${output_dir}")

        string(REPLACE "." "_" module_target_suffix "${module_name}")
        set(stub_target "${stub_target_prefix}_${module_target_suffix}_stub")

        nanobind_add_stub(${stub_target}
            MODULE ${module_name}
            OUTPUT ${output_path}
            PYTHON_PATH $<TARGET_FILE_DIR:${module_target}>
            DEPENDS ${module_target}
        )
    endforeach()
endfunction()

easybind_add_stub_package(pymergetic_easybind
    pymergetic_easybind__init__
    pymergetic.easybind
    pymergetic.easybind.module)
easybind_add_stub_package(pymergetic_easybind_sample
    pymergetic_easybind_sample__init__
    pymergetic.easybind.sample)

if(NOT EASYBIND_SKIP_INSTALL)
    install(TARGETS pymergetic_easybind__module__
        LIBRARY DESTINATION pymergetic/easybind
        RUNTIME DESTINATION pymergetic/easybind
        ARCHIVE DESTINATION pymergetic/easybind
    )
    install(TARGETS pymergetic_easybind__init__
        LIBRARY DESTINATION pymergetic/easybind
        RUNTIME DESTINATION pymergetic/easybind
        ARCHIVE DESTINATION pymergetic/easybind
    )

    install(DIRECTORY src_py/pymergetic
        DESTINATION .
    )
endif()
