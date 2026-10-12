# lh against LVGL: one program, the same scenes drawn by each library's software
# renderer into memory, timed with Google Benchmark. A project of its own and not part
# of lh's build, because it needs LVGL's sources and lh does not.
#
#   cmake -S bench/lvgl -B build-vs-lvgl -G Ninja -DCMAKE_BUILD_TYPE=Release \
#         -DLVGL_DIR=<path to an LVGL checkout>
#   cmake --build build-vs-lvgl
#   build-vs-lvgl/bench_lh_vs_lvgl
#
# Both libraries are built here with the same compiler, -O3 and link-time optimization,
# and neither with -march: what one machine's CPU allows the other is not given.

cmake_minimum_required(VERSION 3.12)
project(lh_vs_lvgl C CXX)

set(LVGL_DIR "" CACHE PATH "An LVGL checkout (the directory with lvgl.h)")
if (NOT EXISTS "${LVGL_DIR}/lvgl.h")
    message(FATAL_ERROR "set -DLVGL_DIR to an LVGL checkout (got '${LVGL_DIR}')")
endif ()
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
endif ()

set(LH_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")

# lh, as it builds itself for a release: -O3 and LTO come from its own options.
set(LH_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LH_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(LH_BUILD_BENCH OFF CACHE BOOL "" FORCE)
set(LH_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${LH_ROOT} lh)

# LVGL from its sources, with the same flags lh is given.
file(GLOB_RECURSE LVGL_SOURCES CONFIGURE_DEPENDS "${LVGL_DIR}/src/*.c")
add_library(lvgl_bench STATIC ${LVGL_SOURCES})
target_include_directories(lvgl_bench PUBLIC "${LVGL_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
target_compile_definitions(lvgl_bench PUBLIC LV_CONF_INCLUDE_SIMPLE)
if (MSVC)
    target_compile_options(lvgl_bench PRIVATE $<$<CONFIG:Release>:/O2 /Oi /Ot /Gy>)
else ()
    target_compile_options(lvgl_bench PRIVATE $<$<CONFIG:Release>:-O3 -DNDEBUG -ffunction-sections -fdata-sections>)
endif ()
include(CheckIPOSupported)
check_ipo_supported(RESULT LH_VS_LVGL_IPO LANGUAGES C CXX)
if (LH_VS_LVGL_IPO)
    set_property(TARGET lvgl_bench PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif ()

# Google Benchmark, the copy lh's own benchmarks use.
set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_INSTALL OFF CACHE BOOL "" FORCE)
set(BENCHMARK_ENABLE_WERROR OFF CACHE BOOL "" FORCE)
add_subdirectory("${LH_ROOT}/bench/lib/benchmark" benchmark)

add_executable(bench_lh_vs_lvgl main.cpp scene_lh.c scene_lvgl.c)
target_link_libraries(bench_lh_vs_lvgl PRIVATE lh lvgl_bench benchmark::benchmark)
if (LH_VS_LVGL_IPO)
    set_property(TARGET bench_lh_vs_lvgl PROPERTY INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
endif ()
