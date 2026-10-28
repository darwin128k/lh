/**
 * @file map.h
 * @brief Backend-private: Win32 message pieces as portable window / input values.
 *
 * Maps and pointer-table helpers. Testable without owning a HWND lifecycle
 * (pointer API lookup and contact table are process-wide).
 */

#ifndef LH_SRC_OS_SYSTEM_WIN_WINDOW_MAP_H
#define LH_SRC_OS_SYSTEM_WIN_WINDOW_MAP_H

#include <lh/bool.h>
#include <lh/os/input/coord.h>
#include <lh/os/input/device/kind.h>
#include <lh/os/input/kind.h>
#include <lh/os/input/pointer/id.h>
#include <lh/os/system/win/user32.h>
#include <lh/os/window/zone.h>
#include <lh/compiler/extern/c.h>

LH_COMPILER_EXTERN_C_BEGIN

/**
 * @brief What ::lh_os_window_zone_t means to Windows (`HT*`).
 *
 * One table so a zone added to the portable enum cannot be forgotten here.
 */
lh_s32_t
lh_os_system_win_window_hit(lh_os_window_zone_t zone);

/** Client x packed in a mouse message's @p lparam, as an event coordinate. */
lh_os_input_coord_t
lh_os_system_win_window_coord_x(lh_os_system_win_lparam_t lparam);

/** Client y of ::lh_os_system_win_window_coord_x. */
lh_os_input_coord_t
lh_os_system_win_window_coord_y(lh_os_system_win_lparam_t lparam);

/** True when this system sends pointer messages (Windows 8+); looked up once. */
lh_bool_t
lh_os_system_win_window_has_pointer_api(lh_void);

/**
 * @brief True when the current mouse message is an echo of a pen/touch already
 *        delivered as a pointer message (skip where pointer API exists).
 */
lh_bool_t
lh_os_system_win_window_is_contact_echo(lh_void);

/**
 * @brief What the system says of @p pointer_id: device, flags, screen point.
 *
 * @return False when the system no longer knows the pointer (@p outs untouched).
 */
lh_bool_t
lh_os_system_win_window_read_pointer(lh_os_system_win_pointer_id_t pointer_id,
                                     lh_os_system_win_wparam_t wparam,
                                     lh_os_system_win_lparam_t lparam,
                                     lh_os_input_device_kind_t *device_kind,
                                     lh_os_system_win_pointer_flags_t *flags,
                                     lh_os_system_win_point_t *at);

/**
 * @brief Map a system pointer id to our contact slot for @p kind.
 *
 * @return False when every id is taken, or the contact was never seen going down.
 */
lh_bool_t
lh_os_system_win_window_contact_pointer(lh_os_system_win_pointer_id_t pointer_id,
                                        lh_os_input_kind_t kind,
                                        lh_os_input_device_kind_t device_kind,
                                        lh_os_input_pointer_id_t *pointer);

/**
 * @brief Top-left of a centered open: work area of the monitor @p owner_hwnd is on
 *        (or the primary), clamped so the title bar stays on-screen.
 */
lh_void
lh_os_system_win_window_centered_origin(const lh_os_system_win_rect_t *window_rect,
                                        lh_os_system_win_hwnd_t owner_hwnd, int *x, int *y);

LH_COMPILER_EXTERN_C_END

#endif /* LH_SRC_OS_SYSTEM_WIN_WINDOW_MAP_H */
