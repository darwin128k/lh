/**
 * @file cb.h
 * @brief Pointer to ::lh_os_controller_handlers_update_fn.
 */

#ifndef LH_OS_CONTROLLER_HANDLERS_UPDATE_CB_H
#define LH_OS_CONTROLLER_HANDLERS_UPDATE_CB_H

#include <lh/os/controller/handlers/update/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_os_controller_handlers_update_cb
 * @brief Pointer to ::lh_os_controller_handlers_update_fn.
 */
#define lh_os_controller_handlers_update_cb lh_ptr_of(lh_os_controller_handlers_update_fn)

#endif /* LH_OS_CONTROLLER_HANDLERS_UPDATE_CB_H */
