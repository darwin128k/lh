/**
 * @file fn.h
 * @brief Timer notify function type for ::lh_os_window_t.
 *
 * Not a pointer type by itself. ::lh_os_window_on_tick_cb is the pointer.
 */

#ifndef LH_OS_WINDOW_ON_TICK_FN_H
#define LH_OS_WINDOW_ON_TICK_FN_H

#include <lh/ptr.h>
#include <lh/void.h>

struct lh_os_window;

/**
 * @typedef lh_os_window_on_tick_fn
 * @brief Called every ::lh_os_window_set_on_tick milliseconds.
 *
 * The window's message loop is what runs this, so it fires only while that window is
 * getting messages: a window behind another one, or one that is closed, does not tick.
 * There is no clock in here on purpose -- an app that measures its own timers from a
 * callback it did not schedule is back to reading somebody else's clock.
 */
typedef lh_void(lh_os_window_on_tick_fn)(struct lh_os_window *self, lh_ptr context);

#endif /* LH_OS_WINDOW_ON_TICK_FN_H */