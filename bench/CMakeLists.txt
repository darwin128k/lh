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
        ${LH_BENCH_SRC_PATH}/lh/memory/std.cpp
        ${LH_BENCH_SRC_PATH}/lh/str/format/uint.cpp
        ${LH_BENCH_SRC_PATH}/lh/str/format/sint.cpp
        ${LH_BENCH_SRC_PATH}/lh/str/format/hex.cpp
        ${LH_BENCH_SRC_PATH}/lh/str/format/text.cpp
        ${LH_BENCH_SRC_PATH}/lh/str/parse/uint.cpp
        ${LH_BENCH_SRC_PATH}/lh/net/ip.cpp
        ${LH_BENCH_SRC_PATH}/lh/io/stream.cpp
)

add_executable(${PROJECT_NAME} ${LH_BENCH_SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${LH_CORE_TARGET} benchmark::benchmark_main)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 14 CXX_STANDARD_REQUIRED ON)

# Windows: put the shared library next to the bench executable so the loader finds it.
if (WIN32)
    add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:${LH_CORE_TARGET}>
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endif ()
