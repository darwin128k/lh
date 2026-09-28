/**
 * @file cb.h
 * @brief Function-pointer aliases for ::lh_os_module_fn.
 */

#ifndef LH_OS_MODULE_CB_H
#define LH_OS_MODULE_CB_H

#include <lh/os/module/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_os_module_start_cb
 * @brief Pointer to ::lh_os_module_start_fn.
 */
#define lh_os_module_start_cb lh_ptr_of(lh_os_module_start_fn)

/**
 * @def lh_os_module_stop_cb
 * @brief Pointer to ::lh_os_module_stop_fn.
 */
#define lh_os_module_stop_cb lh_ptr_of(lh_os_module_stop_fn)

#endif /* LH_OS_MODULE_CB_H */
