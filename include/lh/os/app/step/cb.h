/**
 * @file cb.h
 * @brief Callable wrapper for the step of an application.
 */

#ifndef LH_OS_APP_STEP_CB_H
#define LH_OS_APP_STEP_CB_H

#include <lh/core/util/ptr.h>
#include <lh/os/app/step/fn.h>

/**
 * @def lh_os_app_step_cb
 * @brief Pointer to ::lh_os_app_step_fn.
 *
 * Expands to a function pointer type for the step of an application.
 */
#define lh_os_app_step_cb lh_ptr_of(lh_os_app_step_fn)

#endif /* LH_OS_APP_STEP_CB_H */
