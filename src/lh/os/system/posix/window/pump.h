/**
 * @file pump.h
 * @brief Backend-private: dirty/tick walkers and EWMH helpers for the X pump.
 *
 * Walk the process window list and read `_NET_*` properties. Not portable, but
 * not the event dispatch itself.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_WINDOW_PUMP_H
#define LH_SRC_OS_SYSTEM_POSIX_WINDOW_PUMP_H

#include <lh/bool.h>
#include <lh/numeric/fixed/types.h>
#include <lh/os/system/posix/window/fields.h>

struct lh_os_system_posix_window;
typedef struct lh_os_system_posix_window lh_os_system_posix_window_t;

/** First window that waits to be painted, or ::lh_null. */
lh_os_system_posix_window_t *
lh_os_system_posix_first_dirty(void);

/** First window whose tick is due at @p now, or ::lh_null. */
lh_os_system_posix_window_t *
lh_os_system_posix_first_due(lh_u64_t now);

/** Milliseconds until the earliest tick, or `-1` for none. */
int
lh_os_system_posix_until_tick(lh_u64_t now);

/** `_NET_WORKAREA` into @p x/@p y/@p width/@p height (display fallback if missing). */
lh_void
lh_os_system_posix_workarea(int *x, int *y, int *width, int *height);

/** True when `_NET_WM_STATE` says @p self is maximized. */
lh_bool_t
lh_os_system_posix_window_read_maximized(const lh_os_system_posix_window_t *self);

#endif /* LH_SRC_OS_SYSTEM_POSIX_WINDOW_PUMP_H */
