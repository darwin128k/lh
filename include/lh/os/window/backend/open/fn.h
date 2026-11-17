/**
 * @file fn.h
 * @brief Function type ::lh_os_window_backend_open_fn.
 */

#ifndef LH_OS_WINDOW_BACKEND_OPEN_FN_H
#define LH_OS_WINDOW_BACKEND_OPEN_FN_H

#include <lh/core/compiler/extern/c.h>
#include <lh/core/ptr.h>

struct lh_os_window;

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @typedef lh_os_window_backend_open_fn
 * @brief Create the backend's window for @p window and show it.
 *
 * The backend reads what the application asked for from @p window itself —
 * ::lh_os_window_get_frame, ::lh_os_window_get_corner,
 * ::lh_os_window_get_placement — and stores its own state for the window with
 * ::lh_os_window_set_native before it returns true. It may store it earlier,
 * while the window is being created, when its own messages need it.
 *
 * @param context The backend's context.
 * @param window  The window being opened; never ::lh_null.
 * @param title   Its title; never ::lh_null.
 * @param width   The client width asked for.
 * @param height  The client height asked for.
 * @param owner   The window @p window is a modal child of, or ::lh_null for a
 *                top-level window.
 * @return True when the window is open and its native state is stored.
 */
typedef lh_bool_t(lh_os_window_backend_open_fn)(lh_ptr context, struct lh_os_window *window,
                                                const lh_char_t *title, int width, int height,
                                                struct lh_os_window *owner);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WINDOW_BACKEND_OPEN_FN_H */
