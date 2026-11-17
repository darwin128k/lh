/**
 * @file fn.h
 * @brief Function type ::lh_os_window_backend_get_position_fn.
 */

#ifndef LH_OS_WINDOW_BACKEND_GET_POSITION_FN_H
#define LH_OS_WINDOW_BACKEND_GET_POSITION_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/bool.h>
#include <lh/core/ptr.h>

struct lh_os_window;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_os_window_backend_get_position_fn
 * @brief Read where @p window is from the backend.
 *
 * Optional. Without it ::lh_os_window_get_position answers with the place the
 * window keeps (::lh_os_window_get_rect).
 *
 * @param context The backend's context.
 * @param window  The window; open.
 * @param x       Receives the left edge in screen coordinates; never ::lh_null.
 * @param y       Receives the top edge; never ::lh_null.
 * @return True when the position was read.
 */
typedef lh_bool_t(lh_os_window_backend_get_position_fn)(lh_ptr context,
                                                        const struct lh_os_window *window, int *x,
                                                        int *y);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WINDOW_BACKEND_GET_POSITION_FN_H */
