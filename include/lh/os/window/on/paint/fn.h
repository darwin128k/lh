/**
 * @file fn.h
 * @brief Paint notify function type for ::lh_os_window_t.
 *
 * Not a pointer type by itself. ::lh_os_window_on_paint_cb is the pointer.
 */

#ifndef LH_OS_WINDOW_ON_PAINT_FN_H
#define LH_OS_WINDOW_ON_PAINT_FN_H

#include <lh/ptr.h>
#include <lh/void.h>

struct lh_os_window;

/**
 * @typedef lh_os_window_on_paint_fn
 * @brief Called while the native window is in a paint cycle.
 *
 * On Win32, ::lh_os_window_get_paint_dc is valid for the duration of this call.
 */
typedef lh_void(lh_os_window_on_paint_fn)(struct lh_os_window *self, lh_ptr context);

#endif /* LH_OS_WINDOW_ON_PAINT_FN_H */
