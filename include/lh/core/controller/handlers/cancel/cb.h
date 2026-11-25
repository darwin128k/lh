/**
 * @file cb.h
 * @brief Pointer to ::lh_controller_handlers_cancel_fn.
 */

#ifndef LH_CONTROLLER_HANDLERS_CANCEL_CB_H
#define LH_CONTROLLER_HANDLERS_CANCEL_CB_H

#include <lh/core/controller/handlers/cancel/fn.h>
#include <lh/core/util/ptr.h>

/**
 * @def lh_controller_handlers_cancel_cb
 * @brief Pointer to ::lh_controller_handlers_cancel_fn.
 */
#define lh_controller_handlers_cancel_cb lh_ptr_of(lh_controller_handlers_cancel_fn)

#endif /* LH_CONTROLLER_HANDLERS_CANCEL_CB_H */
