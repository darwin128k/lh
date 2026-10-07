/**
 * @file fn.h
 * @brief Zone query function type for ::lh_os_window_t.
 *
 * Not a pointer type by itself. ::lh_os_window_on_zone_cb is the pointer.
 */

#ifndef LH_OS_WINDOW_ON_ZONE_FN_H
#define LH_OS_WINDOW_ON_ZONE_FN_H

#include <lh/os/window/zone.h>
#include <lh/ptr.h>

struct lh_os_window;

/**
 * @typedef lh_os_window_on_zone_fn
 * @brief Name what the window system should do with a point of the window.
 *
 * @p x and @p y are relative to the client area origin. Answering
 * ::lh_os_window_zone_client is always valid and means "the app handles this one";
 * the OS then sends no non-client mouse messages for the point at all, so a button
 * the app drew there keeps getting ordinary clicks.
 *
 * Called more than once per press — the window system asks before deciding what a
 * press means, and while the pointer moves over a zone. Keep it a hit test and not
 * a decision.
 */
typedef lh_os_window_zone_t(lh_os_window_on_zone_fn)(struct lh_os_window *self, int x, int y, lh_ptr context);

#endif /* LH_OS_WINDOW_ON_ZONE_FN_H */