# What gx costs on a microcontroller: the same program with and without a gx
# screen, built for a Cortex-M4 with no operating system, and the difference
# in flash and RAM. A project of its own and not part of lh's build, because it
# cross-compiles.
#
#   cmake -S bench/mcu -B build-mcu -G Ninja \
#         -DCMAKE_TOOLCHAIN_FILE=$PWD/cmake/toolchain/cortex-m4.cmake
#   cmake --build build-mcu
#   cmake --build build-mcu --target sizes
#
# Nothing runs here: the programs are linked for size, with newlib-nano and no
# system calls, and everything the program does not reach is dropped
# (--gc-sections). The numbers are in bench/mcu/RESULTS.md.

cmake_minimum_required(VERSION 3.12)
project(lh_mcu C)

if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE MinSizeRel)
endif ()

set(LH_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../..")

# lh as a board without an OS builds it: gx on, the operating system off.
set(LH_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LH_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(LH_BUILD_BENCH OFF CACHE BOOL "" FORCE)
set(LH_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(LH_LIBRARY_OPTION_OS OFF CACHE BOOL "" FORCE)
set(LH_LIBRARY_OPTION_OS_WINDOW OFF CACHE BOOL "" FORCE)
set(LH_LIBRARY_OPTION_GL ON CACHE BOOL "" FORCE)
set(LH_LIBRARY_OPTION_GL_FONT_ROBOTO ON CACHE BOOL "" FORCE)
set(LH_LIBRARY_OPTION_THREAD_LOCAL OFF CACHE BOOL "" FORCE)
set(LH_POSITION_INDEPENDENT_CODE OFF CACHE BOOL "" FORCE)
add_subdirectory(${LH_ROOT} lh EXCLUDE_FROM_ALL)

# The baseline: a program that does nothing, with the same C library.
add_executable(mcu_empty mcu_empty.c)

# A gx screen: a button drawn through a band into a panel, pressed and drawn again.
add_executable(mcu_gx mcu_gx.c)
target_link_libraries(mcu_gx PRIVATE lh)

add_custom_target(sizes
        COMMAND ${CMAKE_SIZE} -A -d $<TARGET_FILE:mcu_empty> $<TARGET_FILE:mcu_gx>
        DEPENDS mcu_empty mcu_gx
        VERBATIM)
