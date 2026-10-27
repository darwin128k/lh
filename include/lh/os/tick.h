/**
 * @file tick.h
 * @brief Monotonic clocks for timers and background work.
 *
 * Milliseconds feed ::lh_timer_group_handler. Microseconds are for frame /
 * paint timing. Not wall-clock UTC (::lh_os_timestamp_now).
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_TICK_H
#define LH_OS_TICK_H

#include <lh/config.h>
#include <lh/timer/tick.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/tick.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Milliseconds from an arbitrary origin (wraps as ::lh_tick_t).
 *
 * Truncated form of ::lh_os_tick_ms64; wraps every approximately 49.7 days.
 */
lh_tick_t
lh_os_tick_ms(void);

/**
 * @brief Microseconds from an arbitrary origin.
 *
 * Windows: QueryPerformanceCounter. POSIX: CLOCK_MONOTONIC. Thread-safe.
 * Returns 0 on failure, with the native error captured. For a checked call
 * that preserves output on failure, use lh_os_system_tick_us.
 */
lh_u64_t
lh_os_tick_us(void);

/** 64-bit monotonic milliseconds, same origin as ::lh_os_tick_us. */
lh_u64_t
lh_os_tick_ms64(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_TICK_H */
