/**
 * @file cb.h
 * @brief Pointer to ::lh_os_controller_handlers_handle_fn.
 */

#ifndef LH_OS_CONTROLLER_HANDLERS_HANDLE_CB_H
#define LH_OS_CONTROLLER_HANDLERS_HANDLE_CB_H

#include <lh/os/controller/handlers/handle/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_os_controller_handlers_handle_cb
 * @brief Pointer to ::lh_os_controller_handlers_handle_fn.
 */
#define lh_os_controller_handlers_handle_cb lh_ptr_of(lh_os_controller_handlers_handle_fn)

#endif /* LH_OS_CONTROLLER_HANDLERS_HANDLE_CB_H */
