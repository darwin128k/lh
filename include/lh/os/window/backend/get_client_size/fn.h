/**
 * @file fn.h
 * @brief Function type ::lh_os_window_backend_get_client_size_fn.
 */

#ifndef LH_OS_WINDOW_BACKEND_GET_CLIENT_SIZE_FN_H
#define LH_OS_WINDOW_BACKEND_GET_CLIENT_SIZE_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/bool.h>
#include <lh/core/ptr.h>

struct lh_os_window;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_os_window_backend_get_client_size_fn
 * @brief Read the client size of @p window from the backend.
 *
 * Optional. Without it ::lh_os_window_get_client_size answers with the size the
 * window was last told (::lh_os_window_get_size).
 *
 * @param context The backend's context.
 * @param window  The window; open.
 * @param width   Receives the width; never ::lh_null.
 * @param height  Receives the height; never ::lh_null.
 * @return True when the size was read.
 */
typedef lh_bool_t(lh_os_window_backend_get_client_size_fn)(lh_ptr context,
                                                           const struct lh_os_window *window,
                                                           int *width, int *height);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WINDOW_BACKEND_GET_CLIENT_SIZE_FN_H */
