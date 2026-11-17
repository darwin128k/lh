/**
 * @file fn.h
 * @brief Function type ::lh_os_window_backend_close_fn.
 */

#ifndef LH_OS_WINDOW_BACKEND_CLOSE_FN_H
#define LH_OS_WINDOW_BACKEND_CLOSE_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

struct lh_os_window;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_os_window_backend_close_fn
 * @brief Destroy the backend's window behind @p window.
 *
 * The backend ends in ::lh_os_window_on_native_destroy for @p window — now or
 * when its own destroy arrives. When it cannot (the window is already gone on
 * its side), ::lh_os_window_close makes the up-call itself.
 *
 * @param context The backend's context.
 * @param window  The window to close; open.
 */
typedef lh_void(lh_os_window_backend_close_fn)(lh_ptr context, struct lh_os_window *window);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WINDOW_BACKEND_CLOSE_FN_H */
