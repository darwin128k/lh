/**
 * @file fn.h
 * @brief Timer handler function type of ::lh_os_controller_t:
 *        ::lh_os_controller_handlers_update_fn.
 *
 * Not a pointer type by itself. ::lh_os_controller_handlers_update_cb is the pointer.
 */

#ifndef LH_OS_CONTROLLER_HANDLERS_UPDATE_FN_H
#define LH_OS_CONTROLLER_HANDLERS_UPDATE_FN_H

#include <lh/self.h>
#include <lh/timer/tick.h>

/**
 * @typedef lh_os_controller_handlers_update_fn
 * @brief Called by ::lh_os_controller_update after the recognizer has sent what
 *        was due by @p now.
 *
 * @p self is the controller's context (::lh_os_controller_get_context); @p now
 * is the ::lh_tick_t the update was given. Optional: a controller with no
 * `update_cb` only updates its recognizer.
 */
typedef lh_void(lh_os_controller_handlers_update_fn)(lh_self_ptr self, lh_tick_t now);

#endif /* LH_OS_CONTROLLER_HANDLERS_UPDATE_FN_H */
