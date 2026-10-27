/**
 * @file tick.h
 * @brief Native monotonic clock and finite sleep operations.
 */

#ifndef LH_OS_SYSTEM_TICK_H
#define LH_OS_SYSTEM_TICK_H

#include <lh/config.h>
#include <lh/bool.h>
#include <lh/numeric/fixed/types.h>
#include <lh/compiler/extern/c.h>

#if !LH_LIBRARY_OPTION_OS
#    error "OS clocks require LH_LIBRARY_OPTION_OS"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Read the native monotonic clock without modifying output on failure.
 *
 * The origin is arbitrary and stable for elapsed-time comparisons, not a UTC timestamp.
 *
 * @param out Non-null destination for 64-bit microseconds.
 * @return ::lh_bool_true on success; ::lh_bool_false with a captured native error.
 */
lh_bool_t
lh_os_system_tick_us(lh_u64_t *out);

/**
 * @brief Suspend the calling native thread for a finite interval.
 *
 * Retries interrupted POSIX sleeps using their remaining duration. Windows INFINITE is not
 * used, including for the maximum input value.
 *
 * @param milliseconds Requested sleep interval in milliseconds.
 * @return ::lh_bool_true on completion; ::lh_bool_false on native failure.
 */
lh_bool_t
lh_os_system_sleep_ms(lh_u32_t milliseconds);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_TICK_H */
