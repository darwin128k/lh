/**
 * @file tick.h
 * @brief Monotonic-ish clocks for timers: ::lh_os_tick_ms and ::lh_os_tick_us.
 *
 * Milliseconds feed ::lh_timer_group_handler. Microseconds are for frame /
 * paint timing. Not wall-clock UTC (::lh_os_timestamp_now).
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_TICK_H
#define LH_OS_TICK_H

#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/numeric/fixed/types.h>
#include <lh/timer/tick.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/tick.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Milliseconds from an arbitrary origin (wraps as ::lh_tick_t).
 *
 * Windows: `GetTickCount`. POSIX: `CLOCK_MONOTONIC` when available.
 */
lh_tick_t
lh_os_tick_ms(void);

/**
 * @brief Microseconds from an arbitrary origin.
 *
 * Windows: `QueryPerformanceCounter` / frequency (cached once). POSIX:
 * `CLOCK_MONOTONIC` when available. Returns 0 when the clock is missing.
 */
lh_u64_t
lh_os_tick_us(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_TICK_H */
