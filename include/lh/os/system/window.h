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
#include <lh/os/window/frame.h>
#include <lh/os/window/placement.h>
#include <lh/ptr.h>
#include <lh/timer/tick.h>
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
 * @p frame says whose frame the window has; ::lh_os_window_frame_own creates it with
 * no OS frame, so the client is the whole window and @p width x @p height is the
 * client size. @p corner (> `0`) cuts the window to a rounded region of that radius.
 * @p placement says where it opens; ::lh_os_window_placement_center puts it in the
 * middle of the work area of the monitor it opens on.
 */
lh_os_system_window_handle_t
lh_os_system_window_open(const lh_char_t *title, int width, int height, lh_ptr user,
                         lh_os_system_window_handle_t owner, lh_os_window_frame_t frame, int corner,
                         lh_os_window_placement_t placement);

/**
 * @brief Re-cut @p handle's corners, or make them square again at `0`.
 *
 * A no-op when invalid.
 */
lh_void
lh_os_system_window_set_corner_radius(lh_os_system_window_handle_t handle, int radius);

/**
 * @brief Send @p handle to the taskbar minimized, or put it back.
 *
 * A no-op when invalid.
 */
lh_void
lh_os_system_window_minimize(lh_os_system_window_handle_t handle);

/**
 * @brief Fill the screen with @p handle, or put it back the way it was.
 *
 * A no-op when invalid.
 */
lh_void
lh_os_system_window_set_maximized(lh_os_system_window_handle_t handle, lh_bool_t maximized);

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
 * @enum lh_os_system_window_pump_result
 * @brief Outcome of ::lh_os_system_window_pump_wait.
 */
typedef enum lh_os_system_window_pump_result
{
    lh_os_system_window_pump_quit = 0,
    lh_os_system_window_pump_message = 1,
    lh_os_system_window_pump_timeout = 2
} lh_os_system_window_pump_result_t;

/**
 * @brief Wait up to @p timeout_ms for input, then dispatch pending messages.
 *
 * @p timeout_ms of ::LH_TICK_T_MAX waits forever. Returns
 * ::lh_os_system_window_pump_quit when a quit was posted.
 */
lh_os_system_window_pump_result_t
lh_os_system_window_pump_wait(lh_tick_t timeout_ms);

/**
 * @brief Wake a blocking pump with a quit message.
 */
lh_void
lh_os_system_window_post_quit(void);

/**
 * @brief Mark @p handle dirty so the OS will send a paint.
 */
lh_void
lh_os_system_window_invalidate(lh_os_system_window_handle_t handle);

/**
 * @brief Mark a client rectangle of @p handle dirty (`left`/`top`/`right`/`bottom`,
 *        right and bottom exclusive).
 */
lh_void
lh_os_system_window_invalidate_rect(lh_os_system_window_handle_t handle, int left, int top, int right,
                                    int bottom);

/**
 * @brief Client size of @p handle into @p width and @p height.
 *
 * @return True when the size was read.
 */
lh_bool_t
lh_os_system_window_get_client_size(lh_os_system_window_handle_t handle, int *width, int *height);

/**
 * @brief Where @p handle is on the screen, into @p x and @p y (top-left corner,
 *        screen coordinates).
 *
 * @return True when the position was read.
 */
lh_bool_t
lh_os_system_window_get_position(lh_os_system_window_handle_t handle, int *x, int *y);

LH_COMPILER_EXTERN_C_END

#endif /* LH_OS_SYSTEM_WINDOW_H */
