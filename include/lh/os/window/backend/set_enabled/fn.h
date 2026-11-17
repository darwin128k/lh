/**
 * @file fn.h
 * @brief Function type ::lh_os_window_backend_set_enabled_fn.
 */

#ifndef LH_OS_WINDOW_BACKEND_SET_ENABLED_FN_H
#define LH_OS_WINDOW_BACKEND_SET_ENABLED_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/bool.h>
#include <lh/core/ptr.h>

struct lh_os_window;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_os_window_backend_set_enabled_fn
 * @brief Let @p window take input, or stop it.
 *
 * Optional. Used for the parent of a modal child; without it a modal child
 * does not disable its parent.
 *
 * @param context The backend's context.
 * @param window  The window; open.
 * @param enabled False while a modal child is open.
 */
typedef lh_void(lh_os_window_backend_set_enabled_fn)(lh_ptr context, struct lh_os_window *window,
                                                     lh_bool_t enabled);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WINDOW_BACKEND_SET_ENABLED_FN_H */
