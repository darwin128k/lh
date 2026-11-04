# Parent scope still has the core project name (e.g. lh); this file's project() overwrites PROJECT_NAME.
set(LH_CORE_TARGET ${PROJECT_NAME})
project(${LH_CORE_TARGET}_bench)

set(LH_BENCH_LIB_PATH "${CMAKE_CURRENT_SOURCE_DIR}/lib")
set(LH_BENCH_SRC_PATH "${CMAKE_CURRENT_SOURCE_DIR}/src")

# Skip the bench target if the google/benchmark submodule is not initialized.
if (NOT EXISTS "${LH_BENCH_LIB_PATH}/benchmark/CMakeLists.txt")
    message(WARNING
            "google/benchmark not found — benchmarks disabled.\n"
            "Run: git submodule update --init --recursive")
    return()
endif ()

# Pinned to v1.7.1 (CMake >= 3.5.1, C++14 to build) rather than a later release: newer
# google/benchmark tags raise their own CMake/C++ standard floor (v1.8+ wants CMake 3.10,
# v1.9+ wants C++17 to build), which would sit above what lh itself claims to support
# (CMake 3.12, legacy/XP toolchains — see scripts/build-windows-xp.bat).
#
# The submodule points at github.com/darwin128k/benchmark (branch
# lh/v1.7.1-cstdint-fix), a fork of v1.7.1 carrying one commit on top: a fix for
# src/string_util.cc using int64_t without including <cstdint> directly (it relies
# on the header leaking in transitively), which breaks under newer, stricter
# libstdc++ ("'int64_t' does not name a type") and is still unfixed upstream as of
# the latest release. Bumping this submodule to a newer upstream tag later means
# re-checking whether that fix is still needed and, if so, rebasing it there too.
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "" FORCE)

# Keep benchmark's CRT consistent with this project on MSVC (/MD vs /MT), same as googletest.
if (MSVC)
    set(BENCHMARK_USE_LIBCXX OFF CACHE BOOL "" FORCE)
endif ()

add_subdirectory(${LH_BENCH_LIB_PATH}/benchmark)

set(LH_BENCH_SOURCES
        ${LH_BENCH_SRC_PATH}/main.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/memory/std.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/http.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/array.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/list.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/str.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/str/format/uint.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/str/format/sint.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/str/format/hex.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/str/format/text.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/str/parse/uint.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/str/parse/hex.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/error.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/werror.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/date.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/time.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/datetime.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/str/format/bytes.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/crypto/sha.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/crypto/rijndael.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/compress/deflate.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/str/view.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/str/ptr.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/web/escape.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/str/split/next.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/wstr/ptr.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/wstr/view.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/net/ip.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/net/port.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/net/socket/addr.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/net/modbus.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/io/stream.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/memory/allocator.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/memory/bounds.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/memory/bounds/allocated.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/memory/typed.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/memory/typed/allocated.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/memory/view.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/bit/scan.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/bit/bswap.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/bit/rotate.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/bit/endian.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/math/rect.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/fs/path.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/logger.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/timestamp.cpp
        ${LH_BENCH_SRC_PATH}/lh/core/timer.cpp
)

# The software renderer, the entity tree and the web canvas. One frame of a real
# screen is the number that matters, so these draw the screens bench/lvgl compares.
if (LH_LIBRARY_OPTION_UI)
    list(APPEND LH_BENCH_SOURCES
            ${LH_BENCH_SRC_PATH}/lh/ui/canvas.cpp
            ${LH_BENCH_SRC_PATH}/lh/ui/view.cpp
            ${LH_BENCH_SRC_PATH}/lh/ui/web.cpp
            ${LH_BENCH_SRC_PATH}/lh/ui/web_buf.cpp
    )
endif ()

# Float math (vec/quat/mat4) is only in the library when MATH_FPU is ON.
if (LH_LIBRARY_OPTION_MATH_FPU)
    list(APPEND LH_BENCH_SOURCES
            ${LH_BENCH_SRC_PATH}/lh/core/math/vec3.cpp
            ${LH_BENCH_SRC_PATH}/lh/core/math/mat4.cpp
            ${LH_BENCH_SRC_PATH}/lh/core/math/quat.cpp
    )
endif ()

if (LH_LIBRARY_OPTION_OS)
    # Clocks, files, a directory listing and loopback sockets: system calls through lh.
    list(APPEND LH_BENCH_SOURCES ${LH_BENCH_SRC_PATH}/lh/os/os.cpp)
endif ()

add_executable(${PROJECT_NAME} ${LH_BENCH_SOURCES})

# The core library is built with IPO (LH_IPO_SUPPORTED, root CMakeLists.txt):
# its archive holds IR, so this executable must be linked with IPO as well.
if (LH_IPO_SUPPORTED)
    include(CheckIPOSupported)
    check_ipo_supported(RESULT LH_BENCH_IPO_SUPPORTED LANGUAGES CXX)
    if (LH_BENCH_IPO_SUPPORTED)
        set_property(TARGET ${PROJECT_NAME} PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    endif ()
endif ()

target_link_libraries(${PROJECT_NAME} PRIVATE ${LH_CORE_TARGET} benchmark::benchmark)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED ON)

