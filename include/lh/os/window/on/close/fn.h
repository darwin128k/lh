/**
 * @file fn.h
 * @brief Close notify function type for ::lh_os_window_t.
 *
 * Not a pointer type by itself. ::lh_os_window_on_close_cb is the pointer.
 */

#ifndef LH_OS_WINDOW_ON_CLOSE_FN_H
#define LH_OS_WINDOW_ON_CLOSE_FN_H

#include <lh/os/window/close/reason.h>
#include <lh/ptr.h>
#include <lh/void.h>

struct lh_os_window;

/**
 * @typedef lh_os_window_on_close_fn
 * @brief Called when the native window is gone (API or OS).
 */
typedef lh_void(lh_os_window_on_close_fn)(struct lh_os_window *self,
                                          lh_os_window_close_reason_t reason, lh_ptr context);

#endif /* LH_OS_WINDOW_ON_CLOSE_FN_H */
