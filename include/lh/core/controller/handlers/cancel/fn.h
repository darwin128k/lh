/**
 * @file fn.h
 * @brief Cancel handler function type of ::lh_controller_t:
 *        ::lh_controller_handlers_cancel_fn.
 *
 * Not a pointer type by itself. ::lh_controller_handlers_cancel_cb is the pointer.
 */

#ifndef LH_CONTROLLER_HANDLERS_CANCEL_FN_H
#define LH_CONTROLLER_HANDLERS_CANCEL_FN_H

#include <lh/core/self.h>

/**
 * @typedef lh_controller_handlers_cancel_fn
 * @brief Called by ::lh_controller_cancel once the recognizer has dropped
 *        every gesture: @p self drops what it holds of them too (a press, a drag).
 *
 * @p self is the controller's context (::lh_controller_get_context).
 * Optional: a controller with no `cancel_cb` only resets its recognizer.
 */
typedef lh_void(lh_controller_handlers_cancel_fn)(lh_self_ptr self);

#endif /* LH_CONTROLLER_HANDLERS_CANCEL_FN_H */
