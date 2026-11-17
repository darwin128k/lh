/**
 * @file fn.h
 * @brief Function type ::lh_os_window_backend_set_corner_radius_fn.
 */

#ifndef LH_OS_WINDOW_BACKEND_SET_CORNER_RADIUS_FN_H
#define LH_OS_WINDOW_BACKEND_SET_CORNER_RADIUS_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

struct lh_os_window;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_os_window_backend_set_corner_radius_fn
 * @brief Cut the corners of an open @p window to @p radius.
 *
 * Optional. A radius set before opening is read in `open`; this slot is only for
 * a window that is already open.
 *
 * @param context The backend's context.
 * @param window  The window; open.
 * @param radius  The radius in pixels; `0` is square.
 */
typedef lh_void(lh_os_window_backend_set_corner_radius_fn)(lh_ptr context,
                                                           struct lh_os_window *window, int radius);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WINDOW_BACKEND_SET_CORNER_RADIUS_FN_H */
