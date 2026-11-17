/**
 * @file fn.h
 * @brief Function type ::lh_os_window_backend_invalidate_fn.
 */

#ifndef LH_OS_WINDOW_BACKEND_INVALIDATE_FN_H
#define LH_OS_WINDOW_BACKEND_INVALIDATE_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

struct lh_os_window;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_os_window_backend_invalidate_fn
 * @brief Ask for a paint of the whole of @p window.
 *
 * The paint comes later, through ::lh_os_window_on_native_paint.
 *
 * @param context The backend's context.
 * @param window  The window; open.
 */
typedef lh_void(lh_os_window_backend_invalidate_fn)(lh_ptr context, struct lh_os_window *window);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WINDOW_BACKEND_INVALIDATE_FN_H */
