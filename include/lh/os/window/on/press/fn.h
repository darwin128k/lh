/**
 * @file fn.h
 * @brief Press notify function type for ::lh_os_window_t.
 *
 * Not a pointer type by itself. ::lh_os_window_on_press_cb is the pointer.
 */

#ifndef LH_OS_WINDOW_ON_PRESS_FN_H
#define LH_OS_WINDOW_ON_PRESS_FN_H

#include <lh/ptr.h>
#include <lh/void.h>

struct lh_os_window;

/**
 * @typedef lh_os_window_on_press_fn
 * @brief Called on a primary-button press in client coordinates.
 *
 * @p x and @p y are relative to the client area origin.
 */
typedef lh_void(lh_os_window_on_press_fn)(struct lh_os_window *self, int x, int y, lh_ptr context);

#endif /* LH_OS_WINDOW_ON_PRESS_FN_H */
