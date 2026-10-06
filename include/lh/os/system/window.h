/**
 * @file window.h
 * @brief Platform window primitives behind ::lh_os_window_t.
 *
 * Win32 lives in `src/lh/os/system/win/window.c`. Other backends are added
 * the same way when needed.
 *
 * Requires ::LH_LIBRARY_OPTION_OS_WINDOW.
 */

#ifndef LH_OS_SYSTEM_WINDOW_H
#define LH_OS_SYSTEM_WINDOW_H

#include <lh/bool.h>
#include <lh/char.h>
#include <lh/compiler/extern/c.h>
#include <lh/config.h>
#include <lh/os/system/window/handle.h>
#include <lh/ptr.h>
#include <lh/void.h>

#if !LH_LIBRARY_OPTION_OS_WINDOW
#    error "lh/os/system/window.h requires LH_LIBRARY_OPTION_OS_WINDOW"
#endif

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief Create and show a window.
 *
 * @p user is an ::lh_os_window_t * stored on the native window.
 * @p owner is the owner/parent handle for a modal child, or invalid for
 * top-level.
 */
lh_os_system_window_handle_t
lh_os_system_window_open(const lh_char_t *title, int width, int height, lh_ptr user,
                         lh_os_system_window_handle_t owner);

/**
 * @brief True when @p handle names a live window.
 */
lh_bool_t
lh_os_system_window_is_valid(lh_os_system_window_handle_t handle);

/**
 * @brief Enable or disable @p handle (modal parent disable).
 */
lh_void
lh_os_system_window_set_enabled(lh_os_system_window_handle_t handle, lh_bool_t enabled);

/**
 * @brief Destroy @p handle. No-op when invalid.
 */
lh_void
lh_os_system_window_close(lh_os_system_window_handle_t handle);

/**
 * @brief Pull and dispatch one OS message.
 *
 * @return ::lh_bool_false when a quit was posted, ::lh_bool_true otherwise.
 */
lh_bool_t
lh_os_system_window_pump(void);

/**
 * @brief Wake a blocking pump with a quit message.
 */
lh_void
lh_os_system_window_post_quit(void);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_WINDOW_H */
