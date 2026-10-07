/**
 * @file fn.h
 * @brief Resize notify function type for ::lh_os_window_t.
 *
 * Not a pointer type by itself. ::lh_os_window_on_resize_cb is the pointer.
 */

#ifndef LH_OS_WINDOW_ON_RESIZE_FN_H
#define LH_OS_WINDOW_ON_RESIZE_FN_H

#include <lh/ptr.h>
#include <lh/void.h>

struct lh_os_window;

/**
 * @typedef lh_os_window_on_resize_fn
 * @brief Called after the client area of the window changed size.
 *
 * @p width and @p height are the new client size in pixels. They arrive once per
 * change, including the ones the window system makes by itself — being maximised,
 * restored, or shown for the first time — and including a drag of one of the resize
 * zones named through ::lh_os_window_set_on_zone, which is why it exists: a window
 * that draws its own frame has no frame to announce anything.
 *
 * The window is not damaged by this, and nothing is repainted until the app says so.
 */
typedef lh_void(lh_os_window_on_resize_fn)(struct lh_os_window *self, int width, int height, lh_ptr context);

#endif /* LH_OS_WINDOW_ON_RESIZE_FN_H */