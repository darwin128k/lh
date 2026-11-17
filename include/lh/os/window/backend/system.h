/**
 * @file system.h
 * @brief The system backend of a window: ::lh_os_window_backend_t over
 *        `lh/os/system/window.h`.
 *
 * One table for every operating system LH has a window system for — Win32, X11
 * and AppKit. The link picks which `lh_os_system_window_*` exist; the calls here
 * are portable and only translate the contract into them. The backend's state
 * per window (::lh_os_window_get_native) is the system's handle itself.
 *
 * The table is the host's, filled by ::lh_os_window_backend_system_init, and
 * installed on an application with ::lh_os_app_install_window_backend; nothing
 * installs it by default. The context is not used.
 *
 * Requires ::LH_LIBRARY_OPTION_OS_WINDOW: it is the one part of the window that
 * needs an operating system.
 */

#ifndef LH_OS_WINDOW_BACKEND_SYSTEM_H
#define LH_OS_WINDOW_BACKEND_SYSTEM_H

#include <lh/config.h>
#include <lh/os/system/window/handle.h>
#include <lh/os/window.h>

#if !LH_LIBRARY_OPTION_OS_WINDOW
#    error "lh/os/window/backend/system.h requires LH_LIBRARY_OPTION_OS_WINDOW"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief The system's handle of @p window, or ::LH_OS_SYSTEM_WINDOW_HANDLE_INVALID
 *        when it is not open.
 *
 * For code that talks to the system around the window — a GDI renderer that
 * needs the `HWND` — and only for a window opened on this backend.
 *
 * @param window The window; must not be null.
 * @return The handle.
 */
lh_os_system_window_handle_t
lh_os_window_backend_system_get_handle(const lh_os_window_t *window);

/** @brief `open`: ::lh_os_system_window_open with the window's frame, corner and
 *         placement, the handle stored as the window's native state. */
lh_bool_t
lh_os_window_backend_system_open(lh_ptr context, lh_os_window_t *window, const lh_char_t *title,
                                 int width, int height, lh_os_window_t *owner);

/** @brief `close`: ::lh_os_system_window_close. */
lh_void
lh_os_window_backend_system_close(lh_ptr context, lh_os_window_t *window);

/** @brief `invalidate`: ::lh_os_system_window_invalidate. */
lh_void
lh_os_window_backend_system_invalidate(lh_ptr context, lh_os_window_t *window);

/** @brief `invalidate_rect`: ::lh_os_system_window_invalidate_rect. */
lh_void
lh_os_window_backend_system_invalidate_rect(lh_ptr context, lh_os_window_t *window, int left,
                                            int top, int right, int bottom);

/** @brief `now`: ::lh_os_tick_ms. */
lh_tick_t
lh_os_window_backend_system_now(lh_ptr context);

/** @brief `set_timer`: ::lh_os_system_window_set_timer. */
lh_void
lh_os_window_backend_system_set_timer(lh_ptr context, lh_os_window_t *window, lh_u32_t ms);

/** @brief `set_enabled`: ::lh_os_system_window_set_enabled. */
lh_void
lh_os_window_backend_system_set_enabled(lh_ptr context, lh_os_window_t *window, lh_bool_t enabled);

/** @brief `minimize`: ::lh_os_system_window_minimize. */
lh_void
lh_os_window_backend_system_minimize(lh_ptr context, lh_os_window_t *window);

/** @brief `set_maximized`: ::lh_os_system_window_set_maximized. */
lh_void
lh_os_window_backend_system_set_maximized(lh_ptr context, lh_os_window_t *window,
                                          lh_bool_t maximized);

/** @brief `set_corner_radius`: ::lh_os_system_window_set_corner_radius. */
lh_void
lh_os_window_backend_system_set_corner_radius(lh_ptr context, lh_os_window_t *window, int radius);

/** @brief `get_client_size`: ::lh_os_system_window_get_client_size. */
lh_bool_t
lh_os_window_backend_system_get_client_size(lh_ptr context, const lh_os_window_t *window,
                                            int *width, int *height);

/** @brief `get_position`: ::lh_os_system_window_get_position. */
lh_bool_t
lh_os_window_backend_system_get_position(lh_ptr context, const lh_os_window_t *window, int *x,
                                         int *y);

/** @brief `get_paint_region`: ::lh_os_system_window_get_paint_region. */
lh_bool_t
lh_os_window_backend_system_get_paint_region(lh_ptr context, lh_ptr target, lh_u64_t *area,
                                             lh_u32_t *count);

/** @brief `now_us`: ::lh_os_tick_us. */
lh_u64_t
lh_os_window_backend_system_now_us(lh_ptr context);

/**
 * @brief Fill @p table with every slot of the system backend.
 *
 * @param table The host's table; must not be null, and must outlive every
 *              window opened on it.
 */
lh_void
lh_os_window_backend_system_init(lh_os_window_backend_t *table);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_WINDOW_BACKEND_SYSTEM_H */
