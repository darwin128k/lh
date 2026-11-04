/**
 * @file cb.h
 * @brief Pointer type for the runtime terminate call.
 *
 * Defines ::lh_runtime_terminate_cb as a pointer to ::lh_runtime_terminate_fn,
 * the call an ::lh_runtime_terminate_handler_t stores.
 *
 * @see lh_runtime_terminate_fn
 * @see lh_runtime_terminate_handler_t
 */

#ifndef LH_RUNTIME_TERMINATE_CB_H
#define LH_RUNTIME_TERMINATE_CB_H

#include <lh/core/runtime/terminate/fn.h>
#include <lh/core/util/ptr.h>

/**
 * @def lh_runtime_terminate_cb
 * @brief Pointer-to-callback type for the runtime terminate call.
 *
 * Expands to lh_ptr_of(::lh_runtime_terminate_fn).
 */
#define lh_runtime_terminate_cb lh_ptr_of(lh_runtime_terminate_fn)

#endif /* LH_RUNTIME_TERMINATE_CB_H */
