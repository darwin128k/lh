/**
 * @file fn.h
 * @brief Function type ::lh_os_window_backend_set_timer_fn.
 */

#ifndef LH_OS_WINDOW_BACKEND_SET_TIMER_FN_H
#define LH_OS_WINDOW_BACKEND_SET_TIMER_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

struct lh_os_window;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_os_window_backend_set_timer_fn
 * @brief Run the timer of @p window every @p ms, or stop it at `0`.
 *
 * Optional. Every expiry ends in ::lh_os_window_on_native_tick. Without this
 * slot the window has no timer of its own, and a backend that wants ticks calls
 * ::lh_os_window_on_native_tick itself — from the application's step, say.
 *
 * @param context The backend's context.
 * @param window  The window; open.
 * @param ms      The period; `0` stops the timer.
 */
typedef lh_void(lh_os_window_backend_set_timer_fn)(lh_ptr context, struct lh_os_window *window,
                                                   lh_u32_t ms);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WINDOW_BACKEND_SET_TIMER_FN_H */
