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

# google/benchmark's src/string_util.cc uses int64_t without including <cstdint> directly
# (relies on it leaking in transitively) — breaks with newer, stricter libstdc++
# ("'int64_t' does not name a type"), and is unfixed upstream as of the latest release too.
# Patched here at configure time, in place, rather than as a commit carried in the submodule
# itself: a commit only this machine has isn't fetchable by anyone who clones lh and runs
# `git submodule update --init` — they'd get "not our ref" from GitHub. Idempotent (checked
# before writing) so re-running configure doesn't touch the file once it's already patched.
set(LH_BENCH_STRING_UTIL_CC "${LH_BENCH_LIB_PATH}/benchmark/src/string_util.cc")
file(READ "${LH_BENCH_STRING_UTIL_CC}" LH_BENCH_STRING_UTIL_CONTENT)
if (NOT LH_BENCH_STRING_UTIL_CONTENT MATCHES "#include <cstdint>")
    string(REPLACE "#include <cstdarg>" "#include <cstdarg>\n#include <cstdint>"
            LH_BENCH_STRING_UTIL_CONTENT "${LH_BENCH_STRING_UTIL_CONTENT}")
    file(WRITE "${LH_BENCH_STRING_UTIL_CC}" "${LH_BENCH_STRING_UTIL_CONTENT}")
endif ()

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
        ${LH_BENCH_SRC_PATH}/lh/vector.cpp
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
