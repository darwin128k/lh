/**
 * @file tick.h
 * @brief Monotonic clocks for timers and background work.
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
 * @brief Read wrapping 32-bit monotonic milliseconds.
 *
 * Truncates lh_os_tick_ms64 and wraps about every 49.7 days. Use wrap-aware timer
 * arithmetic.
 *
 * @return Milliseconds modulo 2^32, or zero on native failure.
 */
lh_tick_t
lh_os_tick_ms(void);

/**
 * @brief Read 64-bit monotonic microseconds.
 *
 * Thread-safe. Uses QueryPerformanceCounter on Windows and CLOCK_MONOTONIC on POSIX.
 * Native failures are captured; lh_os_system_tick_us provides a checked output-preserving
 * interface.
 *
 * @return Microseconds from an arbitrary origin, or zero on native failure.
 */
lh_u64_t
lh_os_tick_us(void);

/**
 * @brief Read 64-bit monotonic milliseconds.
 *
 * For elapsed intervals and deadlines, not UTC wall-clock display.
 *
 * @return Milliseconds from the same origin as lh_os_tick_us, or zero on failure.
 */
lh_u64_t
lh_os_tick_ms64(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_TICK_H */
