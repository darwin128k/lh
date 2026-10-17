/**
 * @file cb.h
 * @brief Pointer to ::lh_os_controller_handlers_cancel_fn.
 */

#ifndef LH_OS_CONTROLLER_HANDLERS_CANCEL_CB_H
#define LH_OS_CONTROLLER_HANDLERS_CANCEL_CB_H

#include <lh/os/controller/handlers/cancel/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_os_controller_handlers_cancel_cb
 * @brief Pointer to ::lh_os_controller_handlers_cancel_fn.
 */
#define lh_os_controller_handlers_cancel_cb lh_ptr_of(lh_os_controller_handlers_cancel_fn)

#endif /* LH_OS_CONTROLLER_HANDLERS_CANCEL_CB_H */
