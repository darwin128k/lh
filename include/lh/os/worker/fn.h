/**
 * @file fn.h
 * @brief Background work, completion and payload cleanup callback signature.
 */

#ifndef LH_OS_WORKER_FN_H
#define LH_OS_WORKER_FN_H

#include <lh/void.h>
#include <lh/ptr.h>

/**
 * @typedef lh_os_worker_fn
 * @brief Process the borrowed payload on the callback's dispatching thread.
 * @param user Payload supplied to ::lh_os_worker_submit.
 */
typedef lh_void(lh_os_worker_fn)(lh_ptr user);

#endif /* LH_OS_WORKER_FN_H */
