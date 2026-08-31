cmake_minimum_required(VERSION 3.12)
project(lh
        DESCRIPTION "Light Helper — portable C utility library"
        LANGUAGES C
)

# Project helper modules: version parsing, generated config, and feature options.
include(cmake/version.cmake)
include(cmake/config.cmake)
include(cmake/library_options.cmake)

# ---------------------------------------------------------------------------

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# Keep all targets position-independent (shared is automatic, static becomes reusable in shared link scenarios).
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ---------------------------------------------------------------------------
# Build options
# ---------------------------------------------------------------------------
option(LH_BUILD_SHARED "Build as shared library" ON)
option(LH_BUILD_DOCS "Build documentation" ON)
option(LH_BUILD_TESTS "Build tests" ON)

# ---------------------------------------------------------------------------
# Release optimization options
# ---------------------------------------------------------------------------
option(LH_ENABLE_RELEASE_MAX_OPT
        "Apply aggressive optimizations and try LTO for the lh library in Release only"
        ON)

# ---------------------------------------------------------------------------
# Version (read from VERSION file; substituted into include/lh/config.h via .in)
# ---------------------------------------------------------------------------
lh_read_version("${CMAKE_CURRENT_SOURCE_DIR}/VERSION"
        LH_VERSION_MAJOR LH_VERSION_MINOR LH_VERSION_PATCH)

# Generate `include/lh/config.h` from `include/lh/config.h.in`.
lh_generate_config()

# ---------------------------------------------------------------------------

if (LH_BUILD_DOCS)
    add_subdirectory(docs)
endif ()

# ---------------------------------------------------------------------------

# Library source and public include roots.
set(LH_LIB_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src/${PROJECT_NAME}")
set(LH_LIB_INCLUDE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/include")

# Core library translation units.
set(LH_LIB_SOURCES
        ${LH_LIB_SRC_PATH}/runtime/exception/catch/stack.c
        ${LH_LIB_SRC_PATH}/runtime/allocator.c
        ${LH_LIB_SRC_PATH}/runtime/terminate.c

        ${LH_LIB_SRC_PATH}/numeric/fixed/types.c
        ${LH_LIB_SRC_PATH}/numeric/fixed/limits.c

        ${LH_LIB_SRC_PATH}/memory/allocator.c
        ${LH_LIB_SRC_PATH}/memory/bounds/slice.c
        ${LH_LIB_SRC_PATH}/memory/bounds/allocated.c
        ${LH_LIB_SRC_PATH}/memory/view/slice.c
        ${LH_LIB_SRC_PATH}/memory/bounds.c
        ${LH_LIB_SRC_PATH}/memory/typed.c
        ${LH_LIB_SRC_PATH}/memory/typed/allocated.c
        ${LH_LIB_SRC_PATH}/memory/view.c
        ${LH_LIB_SRC_PATH}/memory/std.c
        ${LH_LIB_SRC_PATH}/memory/raw.c
        ${LH_LIB_SRC_PATH}/memory.c

        ${LH_LIB_SRC_PATH}/exception/origin.c
        ${LH_LIB_SRC_PATH}/exception/catch.c
        ${LH_LIB_SRC_PATH}/exception.c
        ${LH_LIB_SRC_PATH}/version.c
        ${LH_LIB_SRC_PATH}/error.c
        ${LH_LIB_SRC_PATH}/runtime/error.c

        ${LH_LIB_SRC_PATH}/util/wchar/case/fold.c
        ${LH_LIB_SRC_PATH}/util/wstr/case/map.c
        ${LH_LIB_SRC_PATH}/util/wstr/ptr.c
        ${LH_LIB_SRC_PATH}/util/char/case/fold.c
        ${LH_LIB_SRC_PATH}/util/str/case/map.c
        ${LH_LIB_SRC_PATH}/util/str/ptr.c
        ${LH_LIB_SRC_PATH}/util/wchar.c
        ${LH_LIB_SRC_PATH}/str/view.c
        ${LH_LIB_SRC_PATH}/str.c

        ${LH_LIB_SRC_PATH}/lh.c
)

# Build either shared or static target from the same source list.
if (LH_BUILD_SHARED)
    add_library(${PROJECT_NAME} SHARED ${LH_LIB_SOURCES})
    set_target_properties(${PROJECT_NAME} PROPERTIES DEFINE_SYMBOL LH_EXPORTS)
else ()
    add_library(${PROJECT_NAME} STATIC ${LH_LIB_SOURCES})
endif ()

# Expose generated and public include directories to consumers.
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${LH_LIB_INCLUDE_PATH}
)

# Tests (after ${PROJECT_NAME} exists for target_link_libraries).
if (LH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif ()

# Strong release optimizations for the core library (optional; Release only).
if (LH_ENABLE_RELEASE_MAX_OPT)
    if (MSVC)
        target_compile_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Release>:/O2 /GL /Oi /Ot /Gy>
        )
        target_link_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Release>:/LTCG>
        )
    else ()
        target_compile_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Release>:-O3 -DNDEBUG -ffunction-sections -fdata-sections>
        )
        target_link_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Release>:-Wl,--gc-sections>
        )
    endif ()

    include(CheckIPOSupported)
    check_ipo_supported(RESULT LH_IPO_SUPPORTED LANGUAGES C)
    if (LH_IPO_SUPPORTED)
        set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    else ()
        message(STATUS "IPO/LTO not supported for current C toolchain; continuing without IPO.")
    endif ()
endif ()
