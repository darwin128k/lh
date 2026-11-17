/**
 * @file fn.h
 * @brief Function type ::lh_os_app_backend_wait_fn.
 */

#ifndef LH_OS_APP_BACKEND_WAIT_FN_H
#define LH_OS_APP_BACKEND_WAIT_FN_H

#include <lh/core/timer/tick.h>
#include <lh/core/ptr.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_os_app_backend_wait_fn
 * @brief Wait for the next thing to do, at most @p timeout.
 *
 * Deliver what arrived meanwhile — the system's messages, a board's queue —
 * and return. @p timeout is what ::lh_timer_group_until_next said: the time
 * until the next timer is due.
 *
 * @param context The backend's context.
 * @param timeout How long to wait at most, in the clock's units.
 * @return True when the backend was told to quit (a system's quit message).
 */
typedef lh_bool_t(lh_os_app_backend_wait_fn)(lh_ptr context, lh_tick_t timeout);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_APP_BACKEND_WAIT_FN_H */
