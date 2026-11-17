/**
 * @file fn.h
 * @brief Function type ::lh_os_window_backend_set_maximized_fn.
 */

#ifndef LH_OS_WINDOW_BACKEND_SET_MAXIMIZED_FN_H
#define LH_OS_WINDOW_BACKEND_SET_MAXIMIZED_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/bool.h>
#include <lh/core/ptr.h>

struct lh_os_window;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_os_window_backend_set_maximized_fn
 * @brief Maximize @p window, or restore it.
 *
 * Optional. Without it ::lh_os_window_maximize does nothing. The new state comes
 * back through ::lh_os_window_on_native_resize.
 *
 * @param context   The backend's context.
 * @param window    The window; open.
 * @param maximized True to maximize, false to restore.
 */
typedef lh_void(lh_os_window_backend_set_maximized_fn)(lh_ptr context, struct lh_os_window *window,
                                                       lh_bool_t maximized);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WINDOW_BACKEND_SET_MAXIMIZED_FN_H */
