/**
 * @file emit.h
 * @brief Library-private: how a window backend reports an event to the
 *        handler set with ::lh_os_system_window_set_handler.
 */

#ifndef LH_SRC_OS_SYSTEM_WINDOW_EMIT_H
#define LH_SRC_OS_SYSTEM_WINDOW_EMIT_H

#include <lh/os/system/window.h>
#include <lh/os/system/window/event.h>

/**
 * @brief Report an event of @p type about @p window, with the given values
 *        (see ::lh_os_system_window_event_fields); nothing without a handler.
 */
void
lh_os_system_window_emit(lh_os_system_window_handle_t window, lh_uint_t type, lh_int_t x,
                         lh_int_t y, lh_int_t width, lh_int_t height, lh_int_t button,
                         lh_int_t delta);

#endif /* LH_SRC_OS_SYSTEM_WINDOW_EMIT_H */
