/**
 * @file cb.h
 * @brief Pointer alias for ::lh_os_system_window_handler_fn.
 */

#ifndef LH_OS_SYSTEM_WINDOW_HANDLER_CB_H
#define LH_OS_SYSTEM_WINDOW_HANDLER_CB_H

#include <lh/os/system/window/handler/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_os_system_window_handler_cb
 * @brief Pointer to ::lh_os_system_window_handler_fn.
 */
#define lh_os_system_window_handler_cb lh_ptr_of(lh_os_system_window_handler_fn)

#endif /* LH_OS_SYSTEM_WINDOW_HANDLER_CB_H */
