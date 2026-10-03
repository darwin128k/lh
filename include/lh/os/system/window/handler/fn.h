/**
 * @file fn.h
 * @brief Callback signature for window events.
 */

#ifndef LH_OS_SYSTEM_WINDOW_HANDLER_FN_H
#define LH_OS_SYSTEM_WINDOW_HANDLER_FN_H

#include <lh/os/system/window/event.h>
#include <lh/os/system/window/handle.h>
#include <lh/self.h>
#include <lh/void.h>

/**
 * @typedef lh_os_system_window_handler_fn
 * @brief Called from ::lh_os_system_window_pump_messages for each event of
 *        any window.
 *
 * @param self   What was passed to ::lh_os_system_window_set_handler.
 * @param window The window the event is about.
 * @param event  The event; valid during the call only.
 */
typedef lh_void(lh_os_system_window_handler_fn)(lh_self_ptr self,
                                                lh_os_system_window_handle_t window,
                                                const lh_os_system_window_event_t *event);

#endif /* LH_OS_SYSTEM_WINDOW_HANDLER_FN_H */
