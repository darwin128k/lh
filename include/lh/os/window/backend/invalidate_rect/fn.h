/**
 * @file fn.h
 * @brief Function type ::lh_os_window_backend_invalidate_rect_fn.
 */

#ifndef LH_OS_WINDOW_BACKEND_INVALIDATE_RECT_FN_H
#define LH_OS_WINDOW_BACKEND_INVALIDATE_RECT_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

struct lh_os_window;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_os_window_backend_invalidate_rect_fn
 * @brief Ask for a paint of one rectangle of @p window.
 *
 * The paint comes later, through ::lh_os_window_on_native_paint. Right and
 * bottom are exclusive.
 *
 * @param context The backend's context.
 * @param window  The window; open.
 * @param left    Left edge, client coordinates.
 * @param top     Top edge.
 * @param right   Right edge, exclusive.
 * @param bottom  Bottom edge, exclusive.
 */
typedef lh_void(lh_os_window_backend_invalidate_rect_fn)(lh_ptr context,
                                                         struct lh_os_window *window, int left,
                                                         int top, int right, int bottom);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WINDOW_BACKEND_INVALIDATE_RECT_FN_H */
