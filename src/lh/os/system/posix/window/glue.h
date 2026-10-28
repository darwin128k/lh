/**
 * @file glue.h
 * @brief Backend-private: X11 window lifecycle, paint path, and event dispatch.
 *
 * The remaining event-path guts of `posix/window.c` — still not a public API,
 * but no longer file-local `static`.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_WINDOW_GLUE_H
#define LH_SRC_OS_SYSTEM_POSIX_WINDOW_GLUE_H

#include <lh/bool.h>
#include <lh/os/system/posix/window/fields.h>
#include <lh/os/system/posix/x11.h>
#include <lh/os/system/window/handle.h>
#include <lh/os/window/zone.h>

struct lh_os_system_posix_window;
typedef struct lh_os_system_posix_window lh_os_system_posix_window_t;

int
lh_os_system_posix_on_error(lh_os_system_posix_display_t *display,
                            lh_os_system_posix_xerror_event_t *error);

lh_bool_t
lh_os_system_posix_connect(void);

lh_os_system_posix_window_t *
lh_os_system_posix_window_find(lh_os_system_window_handle_t handle);

lh_os_system_posix_window_t *
lh_os_system_posix_window_find_xid(lh_os_system_posix_xid_t xid);

lh_void
lh_os_system_posix_window_unlist(lh_os_system_posix_window_t *self);

lh_void
lh_os_system_posix_window_damage(lh_os_system_posix_window_t *self, int left, int top, int right,
                                 int bottom);

lh_void
lh_os_system_posix_window_cut(lh_os_system_posix_window_t *self);

lh_void
lh_os_system_posix_window_back(lh_os_system_posix_window_t *self);

lh_void
lh_os_system_posix_window_paint(lh_os_system_posix_window_t *self);

lh_bool_t
lh_os_system_posix_window_begin_moveresize(lh_os_system_posix_window_t *self,
                                           lh_os_window_zone_t zone, int x_root, int y_root);

lh_bool_t
lh_os_system_posix_is_repeat(const lh_os_system_posix_xevent_t *release);

lh_bool_t
lh_os_system_posix_window_is_placed(const lh_os_system_posix_window_t *self);

lh_void
lh_os_system_posix_window_wait_placed(lh_os_system_posix_window_t *self);

lh_bool_t
lh_os_system_posix_window_send_wheel(lh_os_system_posix_window_t *self,
                                     const lh_os_system_posix_xevent_t *event);

lh_void
lh_os_system_posix_window_send_button(lh_os_system_posix_window_t *self,
                                      const lh_os_system_posix_xevent_t *event, lh_bool_t down);

lh_void
lh_os_system_posix_window_send_key(lh_os_system_posix_window_t *self,
                                   lh_os_system_posix_xevent_t *event);

lh_void
lh_os_system_posix_dispatch(lh_os_system_posix_xevent_t *event);

#endif /* LH_SRC_OS_SYSTEM_POSIX_WINDOW_GLUE_H */
