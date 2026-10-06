/**
 * @file fn.h
 * @brief Release notify function type for ::lh_os_window_t.
 *
 * Not a pointer type by itself. ::lh_os_window_on_release_cb is the pointer.
 */

#ifndef LH_OS_WINDOW_ON_RELEASE_FN_H
#define LH_OS_WINDOW_ON_RELEASE_FN_H

#include <lh/ptr.h>
#include <lh/void.h>

struct lh_os_window;

/**
 * @typedef lh_os_window_on_release_fn
 * @brief Called on a primary-button release in client coordinates.
 *
 * @p x and @p y are relative to the client area origin. Click synthesis (if
 * any) lives above the OS; this callback alone marks the button up.
 */
typedef lh_void(lh_os_window_on_release_fn)(struct lh_os_window *self, int x, int y, lh_ptr context);

#endif /* LH_OS_WINDOW_ON_RELEASE_FN_H */
