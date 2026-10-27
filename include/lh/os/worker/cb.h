/**
 * @file cb.h
 * @brief Function-pointer alias for ::lh_os_worker_fn.
 */

#ifndef LH_OS_WORKER_CB_H
#define LH_OS_WORKER_CB_H

#include <lh/os/worker/fn.h>
#include <lh/util/ptr.h>

/**
 * @def lh_os_worker_cb
 * @brief Pointer to ::lh_os_worker_fn.
 */
#define lh_os_worker_cb lh_ptr_of(lh_os_worker_fn)

#endif /* LH_OS_WORKER_CB_H */
