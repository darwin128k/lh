cmake_minimum_required(VERSION 3.12)
project(lh
        DESCRIPTION "Light Helper — portable C utility library"
        LANGUAGES C
)

# Project helper modules: version parsing, generated config, and feature options.
include(cmake/version.cmake)
include(cmake/config.cmake)
include(cmake/library_options.cmake)
include(cmake/check_simd.cmake)

# Compile-only SIMD tier probes (see cmake/check_simd.cmake) — sets
# LH_SIMD_HAVE_SSE2 / LH_SIMD_HAVE_AVX2, consumed by lh_generate_config() below.
lh_check_simd()

# ---------------------------------------------------------------------------

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
# Keep all targets position-independent (shared is automatic, static becomes reusable in shared link scenarios).
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ---------------------------------------------------------------------------
# Build options
# ---------------------------------------------------------------------------
option(LH_BUILD_DOCS "Build documentation" ON)
option(LH_BUILD_TESTS "Build tests" ON)
option(LH_BUILD_BENCH "Build microbenchmarks (requires the bench/lib/benchmark submodule)" OFF)
option(LH_BUILD_EXAMPLES "Build the example programs" OFF)

# ---------------------------------------------------------------------------
# Release optimization options
# ---------------------------------------------------------------------------
option(LH_ENABLE_RELEASE_MAX_OPT
        "Apply aggressive optimizations for the lh library in Release only (LTO: LH_LIBRARY_OPTION_LTO)"
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
        ${LH_LIB_SRC_PATH}/runtime/allocator.c
        ${LH_LIB_SRC_PATH}/runtime/terminate.c
        ${LH_LIB_SRC_PATH}/runtime/check.c
        ${LH_LIB_SRC_PATH}/exception.c

        ${LH_LIB_SRC_PATH}/numeric/fixed/types.c
        ${LH_LIB_SRC_PATH}/numeric/fixed/limits.c

        ${LH_LIB_SRC_PATH}/cpu/cache.c
        ${LH_LIB_SRC_PATH}/cpu/id.c
        ${LH_LIB_SRC_PATH}/cpu/simd.c
        ${LH_LIB_SRC_PATH}/cpu/vendor.c

        ${LH_LIB_SRC_PATH}/memory/allocator.c
        ${LH_LIB_SRC_PATH}/memory/sized/allocator.c
        ${LH_LIB_SRC_PATH}/memory/tree.c
        ${LH_LIB_SRC_PATH}/entity.c
        ${LH_LIB_SRC_PATH}/entity/event.c
        ${LH_LIB_SRC_PATH}/entity/2d.c
        ${LH_LIB_SRC_PATH}/entity/3d.c
        ${LH_LIB_SRC_PATH}/entity/screen.c
        ${LH_LIB_SRC_PATH}/entity/label.c
        ${LH_LIB_SRC_PATH}/entity/button.c
        ${LH_LIB_SRC_PATH}/entity/circle.c
        ${LH_LIB_SRC_PATH}/entity/range.c
        ${LH_LIB_SRC_PATH}/entity/slider.c
        ${LH_LIB_SRC_PATH}/entity/knob.c
        ${LH_LIB_SRC_PATH}/entity/spin.c
        ${LH_LIB_SRC_PATH}/entity/scroll.c
        ${LH_LIB_SRC_PATH}/entity/group.c
        ${LH_LIB_SRC_PATH}/entity/option.c
        ${LH_LIB_SRC_PATH}/entity/list.c
        ${LH_LIB_SRC_PATH}/entity/combo.c
        ${LH_LIB_SRC_PATH}/entity/field.c
        ${LH_LIB_SRC_PATH}/entity/keys.c
        ${LH_LIB_SRC_PATH}/entity/image.c
        ${LH_LIB_SRC_PATH}/entity/view.c
        ${LH_LIB_SRC_PATH}/entity/pages.c
        ${LH_LIB_SRC_PATH}/entity/tabs.c
        ${LH_LIB_SRC_PATH}/entity/sheets.c
        ${LH_LIB_SRC_PATH}/entity/container.c
        ${LH_LIB_SRC_PATH}/entity/flex.c
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
        ${LH_LIB_SRC_PATH}/array.c
        ${LH_LIB_SRC_PATH}/list.c
        ${LH_LIB_SRC_PATH}/list/node.c

        ${LH_LIB_SRC_PATH}/ui/canvas.c
        ${LH_LIB_SRC_PATH}/ui/color.c
        ${LH_LIB_SRC_PATH}/ui/style.c
        ${LH_LIB_SRC_PATH}/ui/effect.c
        ${LH_LIB_SRC_PATH}/ui/shadow.c
        ${LH_LIB_SRC_PATH}/ui/blur.c
        ${LH_LIB_SRC_PATH}/ui/glass.c
        ${LH_LIB_SRC_PATH}/ui/theme.c
        ${LH_LIB_SRC_PATH}/ui/font.c
        ${LH_LIB_SRC_PATH}/ui/roboto.c
        ${LH_LIB_SRC_PATH}/math/point.c
        ${LH_LIB_SRC_PATH}/math/point3.c
        ${LH_LIB_SRC_PATH}/math/size.c
        ${LH_LIB_SRC_PATH}/math/rect.c
        ${LH_LIB_SRC_PATH}/math/rect3.c
        ${LH_LIB_SRC_PATH}/math/box.c
        ${LH_LIB_SRC_PATH}/math/isqrt.c
        ${LH_LIB_SRC_PATH}/math/vec2.c
        ${LH_LIB_SRC_PATH}/math/vec3.c
        ${LH_LIB_SRC_PATH}/math/vec4.c
        ${LH_LIB_SRC_PATH}/math/quat.c
        ${LH_LIB_SRC_PATH}/math/mat4.c

        ${LH_LIB_SRC_PATH}/version.c
        ${LH_LIB_SRC_PATH}/date.c
        ${LH_LIB_SRC_PATH}/date/year.c
        ${LH_LIB_SRC_PATH}/date/month.c
        ${LH_LIB_SRC_PATH}/date/month/index.c
        ${LH_LIB_SRC_PATH}/date/day.c
        ${LH_LIB_SRC_PATH}/time.c
        ${LH_LIB_SRC_PATH}/time/second.c
        ${LH_LIB_SRC_PATH}/time/minute.c
        ${LH_LIB_SRC_PATH}/time/hour.c
        ${LH_LIB_SRC_PATH}/datetime.c
        ${LH_LIB_SRC_PATH}/timestamp.c
        ${LH_LIB_SRC_PATH}/error.c
        ${LH_LIB_SRC_PATH}/werror.c
        ${LH_LIB_SRC_PATH}/runtime/error.c

        ${LH_LIB_SRC_PATH}/util/wchar/case/fold.c
        ${LH_LIB_SRC_PATH}/util/wstr/case/map.c
        ${LH_LIB_SRC_PATH}/util/wstr/ptr.c
        ${LH_LIB_SRC_PATH}/util/char/case/fold.c
        ${LH_LIB_SRC_PATH}/util/str/case/map.c
        ${LH_LIB_SRC_PATH}/util/str/ptr.c
        ${LH_LIB_SRC_PATH}/util/wchar.c
        ${LH_LIB_SRC_PATH}/str/view.c
        ${LH_LIB_SRC_PATH}/wstr/view.c
        ${LH_LIB_SRC_PATH}/str.c
        ${LH_LIB_SRC_PATH}/wstr.c
        ${LH_LIB_SRC_PATH}/str/format/uint.c
        ${LH_LIB_SRC_PATH}/str/format/sint.c
        ${LH_LIB_SRC_PATH}/str/format/hex.c
        ${LH_LIB_SRC_PATH}/str/format/text.c
        ${LH_LIB_SRC_PATH}/str/format/bytes.c
        ${LH_LIB_SRC_PATH}/str/parse/text.c
        ${LH_LIB_SRC_PATH}/str/parse/uint.c
        ${LH_LIB_SRC_PATH}/str/parse/hex.c
        ${LH_LIB_SRC_PATH}/str/split/next.c
        ${LH_LIB_SRC_PATH}/str/scanf/next.c
        ${LH_LIB_SRC_PATH}/str/list.c
        ${LH_LIB_SRC_PATH}/wstr/list.c

        ${LH_LIB_SRC_PATH}/fs/path.c
        ${LH_LIB_SRC_PATH}/fs/path/style.c
        ${LH_LIB_SRC_PATH}/fs/stat.c

        ${LH_LIB_SRC_PATH}/logger.c

        ${LH_LIB_SRC_PATH}/crypto/sha.c
        ${LH_LIB_SRC_PATH}/crypto/rijndael.c

        ${LH_LIB_SRC_PATH}/net/ip.c
        ${LH_LIB_SRC_PATH}/net/port.c
        ${LH_LIB_SRC_PATH}/net/socket/addr/ip4.c
        ${LH_LIB_SRC_PATH}/net/socket/addr.c

        ${LH_LIB_SRC_PATH}/io/reader.c
        ${LH_LIB_SRC_PATH}/io/writer.c
        ${LH_LIB_SRC_PATH}/io/stream.c
        ${LH_LIB_SRC_PATH}/io/dgram/reader.c
        ${LH_LIB_SRC_PATH}/io/dgram/writer.c
        ${LH_LIB_SRC_PATH}/io/dgram.c

        ${LH_LIB_SRC_PATH}/lh.c
)

if (LH_LIBRARY_OPTION_OS)
    list(APPEND LH_LIB_SOURCES
            ${LH_LIB_SRC_PATH}/os.c
            ${LH_LIB_SRC_PATH}/os/error.c
            ${LH_LIB_SRC_PATH}/os/alloc.c
            ${LH_LIB_SRC_PATH}/os/system.c
            ${LH_LIB_SRC_PATH}/os/system/error.c
            ${LH_LIB_SRC_PATH}/os/system/error/capture.c
            ${LH_LIB_SRC_PATH}/os/system/str.c
            ${LH_LIB_SRC_PATH}/os/timestamp.c
            ${LH_LIB_SRC_PATH}/os/fs/path.c
            ${LH_LIB_SRC_PATH}/os/fs/stat.c
            ${LH_LIB_SRC_PATH}/os/fs/file.c
            ${LH_LIB_SRC_PATH}/os/fs/dir.c
            ${LH_LIB_SRC_PATH}/os/net.c
            ${LH_LIB_SRC_PATH}/os/net/socket.c
            ${LH_LIB_SRC_PATH}/os/shared.c
            ${LH_LIB_SRC_PATH}/os/module.c
            ${LH_LIB_SRC_PATH}/os/loader.c
    )
    # Kernel backend: the only sources that call Win32 / POSIX directly.
    # Each backend also names the system libraries it links against.
    if (WIN32)
        set(LH_OS_SYSTEM_BACKEND win)
        set(LH_OS_SYSTEM_LINK_LIBS ws2_32)
    else ()
        set(LH_OS_SYSTEM_BACKEND posix)
        set(LH_OS_SYSTEM_LINK_LIBS ${CMAKE_DL_LIBS})
    endif ()
    list(APPEND LH_LIB_SOURCES
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/error/capture.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/error/kind.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/timestamp.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/fs/path.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/fs/file.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/fs/dir.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/fs/stat.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/net.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/net/socket.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/shared.c
            ${LH_LIB_SRC_PATH}/os/system/${LH_OS_SYSTEM_BACKEND}/str.c
    )

    # Native window backend: Win32 (user32.dll) on Windows, Xlib (libX11)
    # on Linux. Cocoa/AppKit for macOS lands under macos/window.c when the
    # backend is added. Each backend is opt-in: when libX11 is missing on
    # Linux (e.g., a CI runner without `libx11-dev` installed), the X11
    # backend is skipped so the rest of the library still builds and links.
    if (LH_LIBRARY_OPTION_OS_WINDOW)
        list(APPEND LH_LIB_SOURCES
                ${LH_LIB_SRC_PATH}/os/window.c
                ${LH_LIB_SRC_PATH}/os/monitor.c
                ${LH_LIB_SRC_PATH}/os/system/window/event.c
                ${LH_LIB_SRC_PATH}/os/system/window/handler.c
        )
        if (WIN32)
            list(APPEND LH_LIB_SOURCES
                    ${LH_LIB_SRC_PATH}/os/system/win/window.c
                    ${LH_LIB_SRC_PATH}/os/system/win/monitor.c
            )
            list(APPEND LH_OS_SYSTEM_LINK_LIBS user32 gdi32)
        elseif (APPLE)
            list(APPEND LH_LIB_SOURCES
                    ${LH_LIB_SRC_PATH}/os/system/macos/window.m
            )
            list(APPEND LH_OS_SYSTEM_LINK_LIBS "-framework AppKit")
            list(APPEND LH_OS_SYSTEM_LINK_LIBS "-framework Foundation")
            # The Cocoa backend file uses Objective-C syntax; tell CMake to
            # compile it with the OBJC language, not C.
            set_source_files_properties(
                    ${LH_LIB_SRC_PATH}/os/system/macos/window.m
                    PROPERTIES LANGUAGE OBJC
            )
        elseif (UNIX AND NOT APPLE)
            # Linux: detect libX11 at configure time. `find_library` is a
            # no-op (and prints nothing) when the library is on the standard
            # search path; on a `libx11-dev`-less CI it skips the X11
            # backend entirely instead of failing the link step.
            find_library(LH_X11_LIBRARY NAMES X11)
            if (LH_X11_LIBRARY)
                list(APPEND LH_LIB_SOURCES
                        ${LH_LIB_SRC_PATH}/os/system/posix/window.c
                        ${LH_LIB_SRC_PATH}/os/system/posix/monitor.c
                )
                list(APPEND LH_OS_SYSTEM_LINK_LIBS ${LH_X11_LIBRARY})
            else ()
                message(STATUS
                        "lh: libX11 not found; X11 window backend disabled"
                        " (install libx11-dev to enable)")
            endif ()
        endif ()
    endif ()
endif ()

# lh is a static library only: it is linked into its host, never loaded as a
# DLL / shared object.
add_library(${PROJECT_NAME} STATIC ${LH_LIB_SOURCES})

# Expose generated and public include directories to consumers.
target_include_directories(
        ${PROJECT_NAME}
        PUBLIC
        ${LH_LIB_INCLUDE_PATH}
        # Library-private headers under src/lh/, included as <lh/...>.
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Kernel backend's system libraries (chosen with LH_OS_SYSTEM_BACKEND above).
if (LH_LIBRARY_OPTION_OS AND LH_OS_SYSTEM_LINK_LIBS)
    target_link_libraries(${PROJECT_NAME} PUBLIC ${LH_OS_SYSTEM_LINK_LIBS})
endif ()

# Vector math, the exact integer root and the box distance take square roots
# through __builtin_sqrtf (src/lh/float/sqrt.h). Without -fno-math-errno GCC/Clang keep a
# call to libm's sqrtf next to the CPU instruction, only to set errno on
# negative input: a dependency a freestanding build cannot satisfy, for a value
# lh never reads.
if (CMAKE_C_COMPILER_ID STREQUAL "GNU" OR CMAKE_C_COMPILER_ID MATCHES "Clang")
    set_source_files_properties(
            ${LH_LIB_SRC_PATH}/math/box.c
            ${LH_LIB_SRC_PATH}/math/isqrt.c
            ${LH_LIB_SRC_PATH}/math/vec2.c
            ${LH_LIB_SRC_PATH}/math/vec3.c
            ${LH_LIB_SRC_PATH}/math/vec4.c
            PROPERTIES COMPILE_OPTIONS -fno-math-errno
    )
endif ()

# Strong release optimizations for the core library (optional; Release only).
if (LH_ENABLE_RELEASE_MAX_OPT)
    if (MSVC)
        # /GL and /LTCG come from LH_LIBRARY_OPTION_LTO below, not from here.
        target_compile_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Release>:/O2 /Oi /Ot /Gy>
        )
    else ()
        target_compile_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Release>:-O3 -DNDEBUG -ffunction-sections -fdata-sections>
        )
        target_link_options(${PROJECT_NAME} PRIVATE
                $<$<CONFIG:Release>:-Wl,--gc-sections>
        )
    endif ()
endif ()

# Link-time optimization (LH_LIBRARY_OPTION_LTO, cmake/library_options.cmake).
# Decided before test/ and bench/ are added: when the archive holds compiler
# IR, its consumers link with IPO too (clang only loads its LTO plugin under
# -flto; GCC's driver loads its own either way).
if (NOT LH_LIBRARY_OPTION_LTO MATCHES "^(OFF|ON|FAT)$")
    message(FATAL_ERROR "LH_LIBRARY_OPTION_LTO must be OFF, ON or FAT (got '${LH_LIBRARY_OPTION_LTO}')")
endif ()
set(LH_IPO_SUPPORTED FALSE)
if (NOT LH_LIBRARY_OPTION_LTO STREQUAL "OFF")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT LH_IPO_SUPPORTED LANGUAGES C)
    if (LH_IPO_SUPPORTED)
        set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    else ()
        message(STATUS "LH_LIBRARY_OPTION_LTO=${LH_LIBRARY_OPTION_LTO}: the C toolchain has no LTO; building without it.")
    endif ()
endif ()
if (LH_IPO_SUPPORTED AND LH_LIBRARY_OPTION_LTO STREQUAL "FAT")
    set(LH_LTO_FAT FALSE)
    if (CMAKE_C_COMPILER_ID STREQUAL "GNU")
        set(LH_LTO_FAT TRUE)
    elseif (CMAKE_C_COMPILER_ID MATCHES "Clang" AND NOT WIN32 AND NOT APPLE
            AND CMAKE_C_COMPILER_VERSION VERSION_GREATER_EQUAL 17)
        include(CheckCCompilerFlag)
        check_c_compiler_flag(-ffat-lto-objects LH_C_HAS_FAT_LTO_OBJECTS)
        set(LH_LTO_FAT ${LH_C_HAS_FAT_LTO_OBJECTS})
    endif ()
    if (LH_LTO_FAT)
        # After CMake's own IPO flags (GCC gets -fno-fat-lto-objects from them),
        # so this one wins.
        target_compile_options(${PROJECT_NAME} PRIVATE $<$<CONFIG:Release>:-ffat-lto-objects>)
        message(STATUS "LH_LIBRARY_OPTION_LTO=FAT: archive holds IR plus machine code.")
    else ()
        message(STATUS "LH_LIBRARY_OPTION_LTO=FAT: no fat LTO objects on ${CMAKE_C_COMPILER_ID} "
                "${CMAKE_C_COMPILER_VERSION}; using ON (IR only, hosts must link with LTO).")
    endif ()
endif ()

# Tests (after ${PROJECT_NAME} exists for target_link_libraries).
if (LH_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif ()

# Benchmarks (after ${PROJECT_NAME} exists for target_link_libraries).
if (LH_BUILD_BENCH)
    add_subdirectory(bench)
endif ()

if (LH_BUILD_EXAMPLES)
    add_subdirectory(example)
endif ()
