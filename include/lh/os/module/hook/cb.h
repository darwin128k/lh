/**
 * @file cb.h
 * @brief ::lh_os_module_hook_cb — pointer to an ::lh_os_module_hook_fn.
 */

#ifndef LH_OS_MODULE_HOOK_CB_H
#define LH_OS_MODULE_HOOK_CB_H

#include <lh/os/module/hook/fn.h>
#include <lh/core/util/ptr.h>

/** @brief Pointer to an ::lh_os_module_hook_fn. */
#define lh_os_module_hook_cb lh_ptr_of(lh_os_module_hook_fn)

#endif /* LH_OS_MODULE_HOOK_CB_H */
