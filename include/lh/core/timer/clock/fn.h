/**
 * @file fn.h
 * @brief Function type ::lh_timer_clock_fn.
 */

#ifndef LH_TIMER_CLOCK_FN_H
#define LH_TIMER_CLOCK_FN_H

#include <lh/core/timer/tick.h>
#include <lh/core/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_timer_clock_fn
 * @brief Read a clock: the time now, in ::lh_tick_t.
 *
 * The clock a timer runs by, as a slot: whoever owns the clock fills it — the
 * operating system's monotonic milliseconds, a board's tick counter, a test's
 * fake. ::lh_os_app_backend_t and ::lh_os_window_backend_t take their time
 * from one.
 *
 * @param context The clock owner's context.
 * @return The time now; it wraps, so compare with ::lh_tick_is_after.
 */
typedef lh_tick_t(lh_timer_clock_fn)(lh_ptr context);

LH_COMPILER_EXTERN_C_END

#endif /* LH_TIMER_CLOCK_FN_H */
