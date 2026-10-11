/**
 * @file connection.h
 * @brief Backend-private: the X connection the window backend opened, for the
 *        surface to share.
 *
 * A surface is created before it knows which window it will be shown in, but a
 * surface in shared memory has to be attached to the one connection the windows
 * are on. That connection is the window backend's; this is how the surface asks
 * for it, and how it learns whether the server refused what it just asked.
 */

#ifndef LH_SRC_OS_SYSTEM_POSIX_CONNECTION_H
#define LH_SRC_OS_SYSTEM_POSIX_CONNECTION_H

#include <lh/bool.h>
#include <lh/os/system/posix/x11.h>

/**
 * @brief The connection the windows are on, or `lh_null` before the first
 *        window has opened one.
 */
lh_os_system_posix_display_t *
lh_os_system_posix_connection_get_display(void);

/**
 * @brief Whether the server has reported an error since the last time this was
 *        asked, and forget it.
 *
 * Errors arrive asynchronously and the backend's handler drops them -- a window
 * the window manager has already closed is not a reason to end the program. A
 * caller that needs to know whether one request worked syncs and then asks this.
 */
lh_bool_t
lh_os_system_posix_connection_take_error(void);

#endif /* LH_SRC_OS_SYSTEM_POSIX_CONNECTION_H */
