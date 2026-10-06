/**
 * @file fn.h
 * @brief Wheel notify function type for ::lh_os_window_t.
 *
 * Not a pointer type by itself. ::lh_os_window_on_wheel_cb is the pointer.
 */

#ifndef LH_OS_WINDOW_ON_WHEEL_FN_H
#define LH_OS_WINDOW_ON_WHEEL_FN_H

#include <lh/ptr.h>
#include <lh/void.h>

struct lh_os_window;

/**
 * @typedef lh_os_window_on_wheel_fn
 * @brief Called on a mouse-wheel tick in client coordinates.
 *
 * @p x and @p y are the cursor in the client area. @p delta is notches
 * (positive away from the user / "up", negative toward / "down").
 */
typedef lh_void(lh_os_window_on_wheel_fn)(struct lh_os_window *self, int x, int y, int delta,
                                          lh_ptr context);

#endif /* LH_OS_WINDOW_ON_WHEEL_FN_H */
