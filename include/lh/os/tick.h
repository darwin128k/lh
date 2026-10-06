/**
 * @file tick.h
 * @brief Monotonic-ish millisecond clock for timers: ::lh_os_tick_ms.
 *
 * Feeds ::lh_timer_group_handler. Not wall-clock UTC (::lh_os_timestamp_now).
 *
 * Requires ::LH_LIBRARY_OPTION_OS.
 */

#ifndef LH_OS_TICK_H
#define LH_OS_TICK_H

#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/timer/tick.h>

#if !LH_LIBRARY_OPTION_OS
#    error "lh/os/tick.h requires LH_LIBRARY_OPTION_OS (CMake: -DLH_LIBRARY_OPTION_OS=ON)"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Milliseconds from an arbitrary origin (wraps as ::lh_tick_t).
 *
 * Windows: `GetTickCount64`. POSIX: `CLOCK_MONOTONIC` when available.
 */
lh_tick_t
lh_os_tick_ms(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_TICK_H */
